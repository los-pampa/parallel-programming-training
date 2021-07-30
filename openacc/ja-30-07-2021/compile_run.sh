#!/bin/bash

#pgcc -O4 ja_seq.c -o ja_seq
pgcc -fast -O4 -acc -ta=tesla ja_acc.c -o ja_acc -Minfo
#echo "Sequential"
#time ./ja_seq > out_seq.txt #113s
echo "Parallel"
time ./ja_acc > out_acc.txt #10s
#diff out_seq.txt out_acc.txt
echo "Done!"
#exit 0