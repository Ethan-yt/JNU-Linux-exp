#!/bin/bash

if [ ! -d $1 ]
then
    echo "$1 is not a directory!"
    exit 1
fi
file='ls *.c'
for f in $file
do
    if [ -f $f ]
    then
        cp $f $1
    fi
done
ls -Sl $1 