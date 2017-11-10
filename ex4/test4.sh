#!/bin/bash

if [ ! -d $1 ]
then
    echo "$1 is not a directory!"
    exit 1
fi
count=$#
cmd=cp
while [ $count -gt 0 ] 
do
    cmd="$cmd \$$count"
    count=`expr $count - 1` 
done
eval $cmd
