#!/bin/bash

array[0]=1  
array[1]=1  

max=10
i=0
sum=0
while [ $i -lt $max ]  
do  
  if [ $i -ge 2 ]
  then
    let array[$i]=array[$i-1]+array[$i-2]  
  fi
  let sum+=array[i]
  echo ${array[$i]}  
  let i++
done

echo $sum
