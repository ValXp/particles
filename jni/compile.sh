#!/bin/bash
if [ ! -e buildinfo.inc ] ; then
	echo "BUILD := 0" > buildinfo.inc
fi
e=`cat buildinfo.inc | head -n1 | sed 's/BUILD := //'`
e=$((e))
export NDK_TOOLCHAIN_VERSION=clang
#export NDK_TOOLCHAIN_VERSION=clang3.2
ndk-build -B -j 16
if [ $? -eq 0 ] ; then
echo "Build $e success"
e=$((e+1))
echo "BUILD := $e" > buildinfo.inc
else
echo "Build $e failed, buildversion not incremented"
fi
