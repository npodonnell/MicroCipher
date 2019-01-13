#!/bin/bash

set -e

MC=bin/microcipher
TD=.test-data
KEY=abc123
FILESIZES=( 1 2 3 4 7 9 31 312 2047 2048 2049 131071 131072 131073 262144 )

mkdir -p $TD

for size in "${FILESIZES[@]}"
do
    datafile=$TD/random$size
    encfile=$datafile.enc
    decfile=$datafile.dec

    echo -n "Generating $size byte file..."
    cat /dev/urandom | head -c $size > $datafile
    echo "DONE"

    echo -n "Testing stdin -> stdout..."
    cat $datafile | $MC -e -k $KEY > $encfile
    cat $encfile | $MC -d -k $KEY > $decfile
    diff -s $datafile $decfile 2>&1 > /dev/null
    rm $encfile $decfile
    echo "DONE"

    echo -n "Testing stdin -> file..."
    cat $datafile | $MC -e -k $KEY -o $encfile
    cat $encfile | $MC -d -k $KEY -o $decfile
    diff -s $datafile $decfile 2>&1 > /dev/null
    rm $encfile $decfile
    echo "DONE"

    echo -n "Testing file -> stdout..."
    $MC -e -k $KEY -i $datafile > $encfile
    $MC -d -k $KEY -i $encfile > $decfile
    diff -s $datafile $decfile 2>&1 > /dev/null
    rm $encfile $decfile
    echo "DONE"

    echo -n "Testing file -> file..."
    $MC -e -k $KEY -i $datafile -o $encfile
    $MC -d -k $KEY -i $encfile -o $decfile
    diff -s $datafile $decfile 2>&1 > /dev/null
    rm $encfile $decfile
    echo "DONE"

    echo -n "Deleting $size byte file..."
    rm $datafile
    echo "DONE"
done

rm -fr $TD

echo "All tests completed successfully!"
