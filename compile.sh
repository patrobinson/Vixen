#!/bin/sh
# Becaude I don't know how to use a makefile
gcc -m64 -D_KERNEL  -mcmodel=kernel -DSVR4 -O2 -c vixen.c
gcc -D_KERNEL -DSVR4 -O2 -c vixen.c

