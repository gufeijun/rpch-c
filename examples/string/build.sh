#!/bin/bash
if [ $# = 0  ]; then
    if [ ! -d "./bin" ]; then
        mkdir bin
    fi
    if [ ! -d "./obj" ]; then
        mkdir obj
    fi

    rm -rf obj/*
    
    hgen -dir . string.gfj
    
    GCCFLAGS="-Wall -O2 -Wno-pointer-to-int-cast -Wno-unused-function"
    INCLUDES="-I../../ -I./"
    
    cd obj
    gcc -c ../../../*.c  $GCCFLAGS
    ar -cvq librpch.a *.o
    cd ..
    
    gcc -o bin/server ./string.rpch.server.c ./server/*.c obj/librpch.a $GCCFLAGS $INCLUDES -lpthread
    gcc -o bin/client ./string.rpch.client.c ./client/*.c obj/librpch.a $GCCFLAGS $INCLUDES

    echo "build success!"
    echo "all binaries are in directory bin!"
elif [ $1 = "clean" ]; then
    rm bin obj -rf
fi
