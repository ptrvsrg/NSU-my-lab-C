cmake_minimum_required(VERSION 3.5)

project(lab10)

add_subdirectory(src)
add_subdirectory(test)

include(CTest)
enable_testing()

set(EXE $<$<BOOL:${WIN32}>:.exe>)
set(DEBUG $<$<BOOL:${WIN32}>:/Debug>)
set(LAB_TEST_EXE ${CMAKE_CURRENT_SOURCE_DIR}/build/test${DEBUG}/lab_test${EXE})
set(LAB_SRC_EXE ${CMAKE_CURRENT_SOURCE_DIR}/build/src${DEBUG}/lab_src${EXE})

add_test(
    NAME ${PROJECT_NAME}
    COMMAND ${LAB_TEST_EXE} ${LAB_SRC_EXE}
)