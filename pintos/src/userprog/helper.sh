#!/bin/bash
make -j8 -C ../examples
make -j8 -C ./

if [ $2 = 'long' ]; then
    echo 'LONG NAME!'
    pintos -v -k --fs-disk=2 -p ../examples/$1 -a fst -- -f -q run 'fst testing one two three'
    #pintos -k --fs-disk=2 -p ../examples/$1 -a fst -- -f -q run 'fst testing one two three'
else
    if [ -n "$2" ]; then
        # If we have many arguments
        echo 'Many arguments!'
        echo $1
        echo $@
        pintos -v -k --fs-disk=2 -p ../examples/$1 -a $1 -- -f -q run "'$@'"
    else
        # If $2 is empty
        #pintos -p ../examples/$1 -a $1 -k --fs-disk=2 -- -f -q run $1
        pintos -v -k --fs-disk=2 -p ../examples/$1 -a $1 -- -f -q run $1
    fi
fi