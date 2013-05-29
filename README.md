Vixen
=====

Basic Solaris LKM rootkit

Here's a Solaris rootkit I was wrote last year. It's not complete, but does the basics. Hides files, processes and modules. Tested on Solaris 10 SPARC and x86.

Running compile.sh should configure your system for use. Running /root/vixen/fox after that will load up the module and open a socket (which isn't hidden) to the big bad world.
