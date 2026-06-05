#!/bin/bash

g++ snake.cpp

if [ $? -eq 0 ]; then
    ./a.out
else
    echo "Sorry the compilation failed:( Please raise an issue."
fi
