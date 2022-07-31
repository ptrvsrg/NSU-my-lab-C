cmake_minimum_required(VERSION 3.5)

project(lab_test)

set(SRC
    testDriver.c
    testLab.h
    testlab10.c
    )
    
add_executable(${PROJECT_NAME} ${SRC})

if("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
	target_compile_options(${PROJECT_NAME} PRIVATE
            /W4
			/WX)
else ()
    target_compile_options(${PROJECT_NAME} PRIVATE
            -Werror
            -Wall
            -Wextra
            -Waddress
            -Warray-bounds
            -Wbuiltin-macro-redefined
            -Wswitch
            -Wunreachable-code
            -pedantic
            -pedantic-errors)
endif ()

find_library(MATH_LIBRARY m)
if(MATH_LIBRARY)
	target_link_libraries(${PROJECT_NAME} PUBLIC ${MATH_LIBRARY})
endif()
