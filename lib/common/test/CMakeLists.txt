cmake_minimum_required(VERSION 3.22)

set(PROJECT_NAME unit_tests)
set(MODULE_NAME "lib_common")

project(${PROJECT_NAME} LANGUAGES C CXX)

find_program(BASH bash HINTS /usr/bin)

# firmware repo path NOTE!!! normalize leaves trailing '/'
cmake_path(SET THIS_REPO_ROOT_PATH NORMALIZE "${CMAKE_CURRENT_SOURCE_DIR}/../../..")

# specify the C standard
set(CMAKE_C_STANDARD 17)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

set(BASE_COVERAGE_FLAGS "-fprofile-arcs -ftest-coverage -fPIC")
set(MISRA_HELPER_FLAGS
    "-Wparentheses -Wbool-compare -Wconversion -Wuninitialized -Wdouble-promotion \
  -Wmisleading-indentation -Winfinite-recursion -Wmissing-include-dirs \
  -Wreturn-type -Wswitch -Wswitch-default -Wswitch-enum -Wunused-local-typedefs \
  -Wformat=2 -Wformat-truncation -Wformat-overflow -Wstrict-prototypes \
  -Wmissing-braces -Wunused-parameter -Wunused-const-variable -Wunused \
  -Wuninitialized")


set(BASE_FLAGS
    "-MMD -pedantic -pedantic-errors -Wall -Wextra -Werror -g ${BASE_COVERAGE_FLAGS}")
set(CMAKE_C_FLAGS "${BASE_FLAGS} ${MISRA_HELPER_FLAGS} -fanalyzer")
set(CMAKE_CXX_FLAGS "${BASE_FLAGS}")

enable_testing()

# path to module sources dir
set(THIS_APP_PATH "${THIS_REPO_ROOT_PATH}lib/common")

# report location
set(REPORT_PATH "${THIS_REPO_ROOT_PATH}reports/${MODULE_NAME}")

# unit tests for this module
set(UTSRC_DIR ${THIS_APP_PATH}/test)

# test support for this module
set(UTSUP_DIR ${THIS_APP_PATH}/test_support)

# path where gtest is built
# Use the test binary dir to avoid stale coverage (.gcda) conflicts
set(TEST_FRAME_BUILD "${CMAKE_BINARY_DIR}/build-gtest")

# use the googletest library
set(GTEST_LOCATION /opt/googletest)
add_subdirectory(${GTEST_LOCATION} ${TEST_FRAME_BUILD})

add_compile_definitions(
    DD_TRACE_EN=1
    DD_UNIT_TESTING=1
    DD_REPLACE_ERROR_HANDLER
)

# ------ Headers ------
# headers for the module we are testing
set(THESE_INCLUDES
    ${THIS_APP_PATH}/api
)

# common library headers
set(COMMON_INCLUDES
)

# ----- ALL INCLUDE PATHS -----
include_directories(

    # test and test support
    ${UTSRC_DIR}
    ${UTSUP_DIR}

    # module specific and common libraries
    ${THESE_INCLUDES}
    ${COMMON_INCLUDES}
)

# ------ Sources ------
# module specific
set(THESE_SOURCES
    ${THIS_APP_PATH}/src/getBytes.c
    ${THIS_APP_PATH}/src/mString.c
    ${THIS_APP_PATH}/src/mPrintf.c
    ${THIS_APP_PATH}/src/mStrToNum.c
    ${THIS_APP_PATH}/src/queues.c
    ${THIS_APP_PATH}/src/utilDevDebug.c
)

# -------- BASE SOURCES (module code only) --------
set(BASE_SOURCES
    # module-specific C files built into both targets
    ${THESE_SOURCES}
)

# test sources for full assertion build
set(SOURCES_FULL
    ${UTSRC_DIR}/test_mString.cpp
    ${UTSRC_DIR}/test_m_printf.cpp
    ${UTSRC_DIR}/test_mStrToNum.cpp
    ${UTSRC_DIR}/test_queues.cpp
    ${UTSRC_DIR}/test_utilDevDebugFull.cpp
    ${UTSRC_DIR}/test_getBytes.cpp
    ${UTSUP_DIR}/utilUnitTestWrapper.cpp
    ${BASE_SOURCES}
)

# test sources for tiny-assert build
set(SOURCES_TINY
    ${UTSRC_DIR}/test_mString.cpp
    ${UTSRC_DIR}/test_m_printf.cpp
    ${UTSRC_DIR}/test_mStrToNum.cpp
    ${UTSRC_DIR}/test_queues.cpp
    ${UTSRC_DIR}/test_utilDevDebugTiny.cpp
    ${UTSRC_DIR}/test_getBytes.cpp
    ${UTSUP_DIR}/utilUnitTestWrapper.cpp
    ${BASE_SOURCES}
)

# --- build for full assertions ---
add_executable(${PROJECT_NAME}_full ${SOURCES_FULL})

target_link_libraries(
    ${PROJECT_NAME}_full
    GTest::gtest_main
)
add_test(NAME common_full
         COMMAND ${PROJECT_NAME}_full
                 --gtest_filter=test_utilDevDebugFull.*)

# -- build for tiny‑assertions
# this target compiles utilDevDebug.c with TINY_ASSERTS_ONLY and will
# run a separate test binary (test_utilDevDebugTiny.cpp)
add_executable(${PROJECT_NAME}_tiny ${SOURCES_TINY})
target_link_libraries(
    ${PROJECT_NAME}_tiny
    GTest::gtest_main
)
# per‑target define: add the flag only when building the tiny executable
# using target_compile_definitions keeps the full target untouched
# (set_source_files_properties earlier set the property globally, which
# caused the full build to also see TINY_ASSERTS_ONLY)
target_compile_definitions(${PROJECT_NAME}_tiny PRIVATE TINY_ASSERTS_ONLY)
# tiny‑case test executable invocation
add_test(NAME common_tiny
         COMMAND ${PROJECT_NAME}_tiny
                 --gtest_filter=test_utilDevDebugTiny.*)

# ---- run tests and analysis tools ---
set(REPORT_OUT "${REPORT_PATH}/CCM.html")
set(COVERAGE_OUT "${REPORT_PATH}/coverage/coverage.html")
set(TEST_REPORT "${REPORT_PATH}/TestResults.json")
set(COV_EXCLUDE_REGEX ".*\\.cpp")
file(MAKE_DIRECTORY "${REPORT_PATH}/coverage")
# full binary post-build: run tests + all analysis
add_custom_command(TARGET ${PROJECT_NAME}_full POST_BUILD
    COMMAND ${PROJECT_NAME}_full --gtest_color=yes --gtest_output=json --output-on-failure
    COMMAND cat test_detail.json > ${TEST_REPORT}
    COMMAND lizard -Eduplicate -m  --CCN 7 --length 40 ${THESE_SOURCES}
    COMMAND lizard -Eduplicate -m  --CCN 7 --length 40 ${THESE_SOURCES} --output_file report.html
    COMMAND cat report.html >> ${REPORT_OUT}
    COMMAND clang-format -style=file:${THIS_REPO_ROOT_PATH}/.clang-format --dry-run --Werror ${THESE_SOURCES} ${COMMON_SOURCES}
    COMMAND gcovr --html-details -o ${COVERAGE_OUT} -r ${THIS_APP_PATH} -e ${COV_EXCLUDE_REGEX} ${CMAKE_BINARY_DIR}/CMakeFiles/${PROJECT_NAME}_full.dir
    COMMAND gcovr -r ${THIS_APP_PATH} -e ${COV_EXCLUDE_REGEX} --print-summary
)
# tiny binary post-build: run tests only (analysis already covered by _full)
add_custom_command(TARGET ${PROJECT_NAME}_tiny POST_BUILD
    COMMAND ${PROJECT_NAME}_tiny --gtest_color=yes --gtest_output=json --output-on-failure
)
# note: if the tests fail but you can't see the test failures then try:
# ./lib/common/test/build/unit_tests_tiny --gtest_color=yes
# ./lib/common/test/build/unit_tests_full --gtest_color=yes

