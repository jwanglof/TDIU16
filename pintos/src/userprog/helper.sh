#!/bin/bash
make -j8 -C ../examples
make -j8 -C ./

if [ $2 = 'long' ]; then
    echo 'LONG NAME!'
    pintos -v -k --fs-disk=2 -p ../examples/$1 -a fst -- -f -q run 'fst testing one two three'
    #pintos -k --fs-disk=2 -p ../examples/$1 -a fst -- -f -q run 'fst testing one two three'
else
    pintos -p ../examples/$1 -a $1 -k --fs-disk=2 -- -f -q run $1
fi