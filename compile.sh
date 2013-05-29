#!/bin/sh
# Because I don't know how to use a makefile
gcc -m64 -D_KERNEL -mcmodel=kernel -DSVR4 -O2 -c vixen.c
gcc -o fox fox.c

mkdir /root/vixen
mv vixen /root/vixen/
mv fox /root/vixen/
