cmake_minimum_required(VERSION 3.22)

set(PROJECT_NAME unit_tests)
set(MODULE_NAME "lib_clocks")


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
set(THIS_APP_PATH "${THIS_REPO_ROOT_PATH}lib/clock")
# report location
set(REPORT_PATH "${THIS_REPO_ROOT_PATH}reports/${MODULE_NAME}")
# path to unit tests for this module
set(UTSRC_DIR ${THIS_APP_PATH}/test)
# path where gtest is built
set(TEST_FRAME_BUILD "${CMAKE_BINARY_DIR}/build-gtest")

# use the googletest library
set(GTEST_LOCATION /opt/googletest)
add_subdirectory(${GTEST_LOCATION} ${TEST_FRAME_BUILD})

add_compile_definitions(
    DD_TRACE_EN=1
    DD_UNIT_TESTING=1
)

# ------ Headers ------
# headers for the module we are testing
set(THESE_INCLUDES
    ${THIS_APP_PATH}/api
)
# common library headers
set(COMMON_INCLUDES
    ${THIS_REPO_ROOT_PATH}lib/drivers/api
    ${THIS_REPO_ROOT_PATH}lib/common/api
)

# ----- ALL INCLUDE PATHS -----
include_directories(
   # test and test support
   ${UTSRC_DIR}
   ${THIS_REPO_ROOT_PATH}lib/common/test_support
   # module specific and common libraries
   ${THESE_INCLUDES}
   ${COMMON_INCLUDES}
)
# ------ Sources ------
# module specific
set(THESE_SOURCES
	${THIS_APP_PATH}/src/clockTick.c
	${THIS_APP_PATH}/src/clockEvents.c
    ${THIS_APP_PATH}/src/dateTime.c
    ${THIS_APP_PATH}/src/utcNorm.c
    ${THIS_APP_PATH}/src/utcTime.c
    ${THIS_APP_PATH}/src/strToUtc.c
)
# common library sources
set(COMMON_SOURCES
    ${THIS_REPO_ROOT_PATH}lib/common/src/mString.c
    ${THIS_REPO_ROOT_PATH}lib/common/src/mPrintf.c
    ${THIS_REPO_ROOT_PATH}lib/common/src/mStrToNum.c
    ${THIS_REPO_ROOT_PATH}lib/common/src/utilDevDebug.c
    ${THIS_REPO_ROOT_PATH}lib/common/src/getBytes.c
)

# -------- ALL SOURCE FILES --------
set(SOURCES
    # test files
    ${UTSRC_DIR}/test_clockTick.cpp
    ${UTSRC_DIR}/test_utcTime.cpp
    ${UTSRC_DIR}/test_utcNorm.cpp
    ${UTSRC_DIR}/test_dateTime.cpp
    # test support common
    ${THIS_REPO_ROOT_PATH}lib/common/test_support/utilUnitTestWrapper.cpp
    # module specific
    ${THESE_SOURCES}
    # common libraries
    ${COMMON_SOURCES}
)
# --- build ---
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(
    ${PROJECT_NAME}
    GTest::gtest_main
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

