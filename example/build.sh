#!/usr/bin/env sh


cc main.c -L/usr/local/lib -lsmallc -Wl,-rpath,/usr/local/lib -o main
./main
