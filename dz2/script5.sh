#!/bin/bash

function FibboWithRecursion() {                 # function for counting Fibbo's number without recursion
    local n=$1
    if [ $n -le 2 ]                             # corner cases
    then
        return 1
    else
        FibboWithRecursion $[ $n - 1 ]          # F(n - 1)
        local prev=$?
        FibboWithRecursion $[ $n - 2 ]          # F(n - 2)
        local prevprev=$?
        local result=$[ $prev + $prevprev ]     # F(n) = F(n - 1) + F(n - 2)
        return $result
    fi
}

if [ $# -eq 0 ]                                 # if there are no command-line arguments
then
    # print promt
    echo -n "Please enter an integer N to count Fibbo_N number (WARNING! Correct only for 0 < N < 14) -> "
    read N                                      # read N
else                                            # else at least one c.-l. argument
    N=$1                                        # copy argument to N
fi

FibboWithRecursion $N
echo $?
