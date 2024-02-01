#!/bin/bash

function FibboWithoutRecursion() {   # function for counting Fibbo's number without recursion
    n=$1                             # iteration variable
    a=1                              # previous number
    b=1                              # current number

    while [ $n -gt 2 ]               # while loop
    do
        n=$[ $n - 1 ]                # decrement iteration variable
        tmp=$[ $a + $b ]             # tmp = prev + cur
        a=$b                         # prev = cur
        b=$tmp                       # cur = tmp (= prev + cur)
    done

    return $b                        # return current number
}

if [ $# -eq 0 ]                      # if there are no command-line arguments
then
    # print promt
    echo -n "Please enter an integer N to count Fibbo_N number (WARNING! Correct only for 0 < N < 14) -> "
    read N                           # read N
else                                 # else at least one c.-l. argument
    N=$1                             # copy argument to N
fi

FibboWithoutRecursion $N
echo $?
