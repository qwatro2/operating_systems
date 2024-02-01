#!/bin/bash

echo -n "Please enter an integer -> "	# print promt
read int								# read integer


echo "You enter the number $int"		# print info about entered number

if [ $int -gt 100 ]; then
	echo "$int is bigger than 100."		# print extra info about entered number
fi
