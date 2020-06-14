#!/bin/bash

#cc test_lib.c   -L../src/ -lPUBsrc -Wl,-rpath=../src/
cc test_lib.c   -L../ -lPUBFrame_Lib -Wl,-rpath=../
