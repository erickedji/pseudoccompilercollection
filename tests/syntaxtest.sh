#!/bin/bash

#
# KEDJI Komlan Akpédjé <eric_kedji@yahoo.fr>
# Syntax check for the PseudoC parser.
#

# Total number of tests
declare -i nb_tests=10
# Total number of successfull tests
declare -i nb_passed=0
# Total number of unsuccessfull tests
declare -i nb_failed=0
declare -i n

echo " Syntax tests for the PseudoC Parser"
echo

if [ $# == 1 ]; then
    nb_tests=$1
fi

cd ..
echo "Stress testing the PseudoC compiler with $nb_tests tests..."
for (( n=1; n <= nb_tests; n++)); do
    ./generator | ./pseudocc 1>/dev/null 2>/dev/null
    if [ $? != -1 ]; then
        ((nb_passed++));
        echo -e " Test $n:\tSUCCESS."
    else
        echo -e " Test $n:\tFAILURE."
    fi
done

nb_failed=nb_tests-nb_passed
echo 
echo $nb_tests Runned.
echo $nb_passed Passed.
echo $nb_failed Failed.

