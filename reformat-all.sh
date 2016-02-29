#!/bin/bash

for i in *.c
do
    uncrustify --replace --no-backup $i
done
for i in *.h
do
    uncrustify --replace --no-backup $i
done
