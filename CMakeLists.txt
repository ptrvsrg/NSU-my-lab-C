cmake_minimum_required(VERSION 3.5)

project(lab10 C)

add_subdirectory(src)
add_subdirectory(test)

enable_testing()

set(CONFIG $<$<BOOL:${WIN32}>:/${CMAKE_BUILD_TYPE}>)
set(LAB_TEST_EXE ${CMAKE_BINARY_DIR}/test${CONFIG}/lab_test)
set(LAB_SRC_EXE ${CMAKE_BINARY_DIR}/src${CONFIG}/lab_src)

add_test(
    NAME ${PROJECT_NAME}
    COMMAND ${LAB_TEST_EXE} ${LAB_SRC_EXE}
)