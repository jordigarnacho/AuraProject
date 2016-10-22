#!/bin/bash

prog=$1

./build.sh -p Unix-base -t build-sdk
source out/Unix-base/staging/native-wrapper.sh
out/Unix-base/staging/usr/bin/$prog
