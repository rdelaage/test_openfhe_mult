#!/bin/env bash

mkdir -p test
mkdir -p build
cd build
cmake ..
if [ $? -ne 0 ]
then
	exit 1
fi
make -j $(nproc)
if [ $? -ne 0 ]
then
	exit 1
fi
cd ..

./build/generator
./build/encryption
