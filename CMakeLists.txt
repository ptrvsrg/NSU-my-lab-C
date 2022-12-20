cmake_minimum_required(VERSION 3.5)

project(lab10 C)

add_subdirectory(src)
add_subdirectory(test)

include(CTest)
enable_testing()

set(EXE $<$<BOOL:${WIN32}>:.exe>)
set(CONFIG $<$<BOOL:${WIN32}>:/${CMAKE_BUILD_TYPE}>)
set(LAB_TEST_EXE ${CMAKE_BINARY_DIR}/test${CONFIG}/lab_test${EXE})
set(LAB_SRC_EXE ${CMAKE_BINARY_DIR}/src${CONFIG}/lab_src${EXE})

add_test(
    NAME ${PROJECT_NAME}
    COMMAND ${LAB_TEST_EXE} ${LAB_SRC_EXE}
)