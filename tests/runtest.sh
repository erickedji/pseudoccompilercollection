#!/bin/bash

# KEDJI Komlan Akpédjé <eric_kedji@yahoo.fr>
# This script is part os a (nice) testing framework
# for PseudoC programs.
# It takes 2 arguments:
#   - a source file to test
#   - a description of the test
# It the compiles the source file with gcc and pseudocc,
# runs the two programs, and diffs the outputs.

if [ $# != 2 ]; then
    echo -e "\nUsage:\t" $0 "<test_source_file> <test_description>\n"
    exit 2
fi

# source file to test
declare source=$1
# test description
declare desc=$2
# where to store the generated pseudoc source file
declare pc_sources=pc_src

declare -i exitcode

echo -e " -> " $desc

# compile the source file using gcc
gcc -D_C_PROGRAM_ -o gcc_exe $source libpc.c

# compile the source file using pseudocc
# (remove the preprocessor line directives with grep) [ugly]
cpp $source | grep -v '\(^#\)' | grep -v '\(^$\)' > $pc_sources/$source.pc
../pseudocc $pc_sources/$source.pc 1>/dev/null 2>/dev/null

# generate bytecode and interpret the intermediate code
../pseudoci PseudoCIntermediateCode.s > PCC_Output.dat 
if [ $? != 0 ]; then
    echo ' Warning: abnormal termination of the PseudoC executable'
fi

# run the program compiled by gcc
./gcc_exe > GCC_Output.dat
if [ $? != 0 ]; then
    echo ' Warning: abnormal termination of the C executable'
fi

# compare the two outputs
./identical GCC_Output.dat PCC_Output.dat

exitcode=$?
if [ $exitcode == 0 ]; then
    echo -e ' SUCCESS.' 
else
    echo -e ' FAILED.' 
fi

# clean up
rm -f PCC_Output.dat GCC_Output.dat PseudoCIntermediateCode.s PseudoCBinary.pcb

exit $exitcode

