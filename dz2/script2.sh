#!/bin/bash

echo -n "Please enter an integer -> "	# print promt
read int								# read integer

if [ $((int % 2)) -eq 0 ]; then
	echo "$int is even."				# if int % 2 == 0, print int is even
else
	echo "$int is odd."					# else (= if int % 2 == 1), print int is odd
fi
