cmake_minimum_required(VERSION 3.22)

set(MY_PROJECT_NAME Example_C_gtest)
project(${MY_PROJECT_NAME} LANGUAGES C CXX)

find_program(BASH bash HINTS /usr/bin)

# path to the root of the project repository relative to this file
# project repo path NOTE!!! normalize leaves trailing '/'
cmake_path(SET THIS_REPO_ROOT_PATH NORMALIZE "${CMAKE_CURRENT_SOURCE_DIR}/../..")

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
set(THIS_APP_PATH "${THIS_REPO_ROOT_PATH}example/application")

# report location
set(REPORT_PATH "${THIS_REPO_ROOT_PATH}reports/${MODULE_NAME}")

# path to unit tests for this module
set(UTSRC_DIR "${THIS_REPO_ROOT_PATH}example/test")

# test support for this module
set(UTSUP_DIR "${THIS_REPO_ROOT_PATH}example/test_support")

# path where gtest is built
# Use the test binary dir to avoid stale coverage (.gcda) conflicts
set(TEST_FRAME_BUILD "${CMAKE_BINARY_DIR}/gtest_build")

# use the googletest library
set(GTEST_LOCATION /opt/googletest)
add_subdirectory(${GTEST_LOCATION} ${TEST_FRAME_BUILD})

add_compile_definitions(
    DD_TRACE_EN=1
    DD_UNIT_TESTING=1
)

# ------ Headers ------
# headers specific to this api that we are testing
set(THESE_INCLUDES
    ${THIS_APP_PATH}
)

# common library headers
set(COMMON_INCLUDES
    ${THIS_REPO_ROOT_PATH}config
    ${THIS_REPO_ROOT_PATH}lib/common/api
    ${THIS_REPO_ROOT_PATH}lib/drivers/api
)

# ----- ALL INCLUDE PATHS -----
include_directories(

    # test and test support
    ${UTSRC_DIR}
    ${UTSUP_DIR}

    # module specific and common libraries
    ${THESE_INCLUDES}
    ${COMMON_INCLUDES}
    # test support headers
    ${THIS_REPO_ROOT_PATH}lib/common/test_support
    # stub
    ${THIS_REPO_ROOT_PATH}HAL
)

# ------ Sources ------
# module specific
set(THESE_SOURCES
    ${THIS_APP_PATH}/counter.c
    ${THIS_APP_PATH}/evaluate.c
    ${THIS_APP_PATH}/helloWorld.c
)

# common library sources
set(COMMON_SOURCES
    ${THIS_REPO_ROOT_PATH}lib/common/src/utilCrc32.c
    ${THIS_REPO_ROOT_PATH}lib/drivers/src/gpio.c
    ${THIS_REPO_ROOT_PATH}config/autoVersion.c
    ${THIS_REPO_ROOT_PATH}lib/common/src/mString.c
    ${THIS_REPO_ROOT_PATH}lib/common/src/utilDevDebug.c
)

# -------- ALL SOURCE FILES --------
set(SOURCES
    # test files
    ${UTSRC_DIR}/test_counter.cpp
    ${UTSRC_DIR}/test_counterExample2.cpp
    ${UTSRC_DIR}/test_crc32.cpp
    ${UTSRC_DIR}/test_evaluate.cpp
    ${UTSRC_DIR}/test_gpio.cpp
    ${UTSRC_DIR}/test_helloWorld.cpp
    ${UTSRC_DIR}/test_main.cpp
    # test support files
    ${UTSUP_DIR}/helperMain.cpp
    # module specific
    ${THESE_SOURCES}
    # common libraries
    ${COMMON_SOURCES}
    ${THIS_REPO_ROOT_PATH}lib/common/test_support/utilUnitTestWrapper.cpp
)

# --- build ---
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(
    ${PROJECT_NAME}
    GTest::gmock_main
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})

# ---- run tests and analysis tools ---
set(REPORT_OUT "${REPORT_PATH}/CCM.html")
set(COVERAGE_OUT "${REPORT_PATH}/coverage/coverage.html")
set(TEST_REPORT "${REPORT_PATH}/TestResults.json")
set(COV_EXCLUDE_REGEX ".*\\.cpp")
file(MAKE_DIRECTORY "${REPORT_PATH}/coverage")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${PROJECT_NAME} --gtest_color=yes --gtest_output=json --output-on-failure
    COMMAND cat test_detail.json > ${TEST_REPORT}
    COMMAND lizard -Eduplicate -m --CCN 7 --length 40 ${THESE_SOURCES}
    COMMAND lizard -Eduplicate -m --CCN 7 --length 40 ${THESE_SOURCES} --output_file report.html
    COMMAND cat report.html >> ${REPORT_OUT}
    COMMAND clang-format -style=file:${THIS_REPO_ROOT_PATH}/.clang-format --dry-run --Werror ${THESE_SOURCES} ${COMMON_SOURCES}
    COMMAND gcovr --html-details -o ${COVERAGE_OUT} -r ${THIS_APP_PATH} -e ${COV_EXCLUDE_REGEX}
    COMMAND gcovr -r ${THIS_APP_PATH} -e ${COV_EXCLUDE_REGEX} --print-summary
)
