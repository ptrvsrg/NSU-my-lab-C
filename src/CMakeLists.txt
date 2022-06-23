cmake_minimum_required(VERSION 3.5)

project(lab10)

set(SRC
    andrew.c
    andrew.h
    error.c
    error.h
    point.c
    point.h
    stack.c
    stack.h
    vector.c
    vector.h
    main.c
    )

add_executable(lab10 ${SRC})