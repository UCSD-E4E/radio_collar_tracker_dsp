#!/bin/bash

echo ""
echo ">>> ALERT: Updated use of build_and_run.sh: use argument '-v' for traces <<<"
echo ""


if [ "$1" == "-v" ]; then
    set -v
    make clean
    make verbose
    ./sdr_record -r 1 -s 2000000 -o ./data -g 5 -f 2400000000
else
    make clean
    make
    ./sdr_record -r 1 -s 2000000 -o ./data -g 5 -f 2400000000 > /dev/null
fi
