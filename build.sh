#!/bin/sh

make clean

phpize5

./configure --enable-agsat

make
