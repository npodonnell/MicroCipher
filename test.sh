#!/bin/bash

MC=bin/microcipher
TD=test-data
KEY=abc123

time $MC -e -i $TD/test.txt -o $TD/test.txt.enc -k$KEY
echo "Encrypting returned $?"

time $MC -d -i $TD/test.txt.enc -o $TD/test.txt.dec -k$KEY
echo "Decrypting returned $?"

diff $TD/test.txt $TD/test.txt.dec

if [ $? != "0" ]; then
    echo "Diff failed!"
fi

echo "Output---------------------------------------"
cat $TD/test.txt.dec
echo "---------------------------------------------"