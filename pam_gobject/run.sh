#! /bin/sh
export LD_LIBRARY_PATH=`pwd`/.libs:$LD_LIBRARY_PATH
export GI_TYPELIB_PATH=`pwd`
gjs test.js
