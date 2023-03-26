#!/bin/bash

BUILD_DIR="$(pwd)/build"
APP_NAME=sparse

if [ ! -f generate_test_file.py ]; then
    echo "generate_test_file.py does not exist"
    exit 1
fi

echo "### Building sparse binary ###"
make clean && make

echo "### Generating fileA ####"
./generate_test_file.py

cd $BUILD_DIR

echo "### Creating sparse fileB from fileA ###"
./$APP_NAME "fileA" "fileB"
stat fileA >> result.txt
echo $'\n' >> result.txt

stat fileB >> result.txt
echo $'\n' >> result.txt

echo "### Archiving fileA and fileB with gzip ###"
gzip -k fileA
stat fileA.gz >> result.txt
echo $'\n' >> result.txt

gzip -k fileB
stat fileB.gz >> result.txt
echo $'\n' >> result.txt

echo "### Unzipping fileB.gz and creating new sparse fileC from it ###"
gzip -cd fileB.gz | ./$APP_NAME fileC
stat fileC >> result.txt
echo $'\n' >> result.txt

echo "### Creating sparse fileD from fileA with block size 100 ###"
./$APP_NAME fileA fileD -b 100
stat fileD >> result.txt

cd ..

echo "### Done. Results are in $BUILD_DIR ###"