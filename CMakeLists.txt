cmake_minimum_required(VERSION 3.5)

project(lab10)

add_subdirectory(src)
add_subdirectory(test)

include(CTest)
enable_testing()

set(LAB_TEST_EXE ${CMAKE_CURRENT_SOURCE_DIR}/build/test$<$<BOOL:${WIN32}>:/Debug>/lab_test$<$<BOOL:${WIN32}>:.exe>)
set(LAB_SRC_EXE ${CMAKE_CURRENT_SOURCE_DIR}/build/src$<$<BOOL:${WIN32}>:/Debug>/lab_src$<$<BOOL:${WIN32}>:.exe>)

add_test(
    NAME ${PROJECT_NAME}
    COMMAND ${LAB_TEST_EXE} ${LAB_SRC_EXE}
)