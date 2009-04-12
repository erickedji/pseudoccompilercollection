#!/bin/bash

# KEDJI Komlan Akpédjé <eric_kedji@yahoo.fr>
# Test cases for the PseudoC Compiler Collection.

# Total number of tests runned
declare -i nb_tests=0
# Total number of successfull tests
declare -i nb_passed=0
# Total number of unsuccessfull tests
declare -i nb_failed=0

# run a test case
# $1 = source file
# $2 = description
function t()
{
    ./runtest.sh $1.c "$2"
    if [ $? == 0 ]; then
        ((nb_passed++))
    fi
    ((nb_tests++))
    echo
}

echo -e '\n\t Automated tests for the PseudoC Compiler Collection\n'

t io        'IO Routines \n(write_string, write_int)'
t boolean   'Boolean operators \n(AND, OR, NOT, <, >, <=, >=, ==, !=, Short-circuit)'
t math      'Math operators \n(+, -, /, *, %)'
t flow      'Flow control \n(if, if else, while)'
t array     'Arrays'
t classics  'Language flamewar classic recursive functions \n(fib, fact, ack, prime, gcd, fastexpt)'
t scope     'Identifier scope \n(locals and global vars)'

nb_failed=nb_tests-nb_passed
echo ' Summary:'
echo -e "\t $nb_tests test(s) runned."
echo -e "\t $nb_passed test(s) passed."
echo -e "\t $nb_failed test(s) failed."

exit $nb_failed

#/*
# * Points to test:
# * - argument types verification in function usage
# * - check for return statement presence / absense
# *  (warn for function not always returning a value?
# *  seems *hard* without an ast!!!)
# * - clearing the expr_stack of instructions reduced to expressions
# * - static local vars
# * - array access check for bounds (add debugging ops?)
# * - check that main is defined and has the correct prototype
# * - warn unused vars?
# */ 

