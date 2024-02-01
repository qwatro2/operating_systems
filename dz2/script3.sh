#!/bin/bash

i=0                                     # loop variable

while [ true ]                          # infinite loop
do 
    if [ $[ i % 2 ] -eq 0 ]
    then
        echo "Stop me with Ctrl+C"      # promt for stopping
    else
        echo "Why you don't stop me?"   # promt for haha
    fi
    i=$[ $i + 1 ]                       # increment loop variable
    sleep 0.5                           # sleep for 0.5 seconds
done
