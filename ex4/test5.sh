#!/bin/bash

if [ ! -d $1 ]
then
    echo "$1 is not a directory!"
    exit 1
fi
count=2
cd $1
cmd=cat
while [ $count -le $# ] 
do
    cmd="$cmd \$$count"
    count=`expr $count + 1` 
done
eval $cmd