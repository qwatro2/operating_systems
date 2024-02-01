#!/bin/bash

for i in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
do
    correct=$(./script4.sh $i)
    unknown=$(./script5.sh $i)
    echo -n "N = $i ; correct = $correct ; unknown = $unknown; equal = "
    if [ $correct -eq $unknown ]
    then
        echo "true"
    else
        echo "false"
    fi
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
done