# Thanks to https://github.com/bsamseth/cpp-project/ for the boiler plate

cmake_minimum_required(VERSION 2.8)

# Set project name here.
project(C_LINKED_LIST)
enable_language(C CXX)

# Set version number (change as needed). These definitions are available
# by including "listConfig.h" in the source.
# See listConfig.h.in for some more details.
set(LIST_VERSION_MAJOR 0)
set(LIST_VERSION_MINOR 1)


# Include stuff. No change needed.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
include(ConfigSafeGuards)
include(Colors)


# --------------------------------------------------------------------------------
#                          Compile flags (change as needed).
# --------------------------------------------------------------------------------
# Set the C++ standard you wish to use (will apply to all files).
# If you do not use any features that limits the standard required,
# you could omit this line.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")


# Things to always include as flags. Change as needed.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Wall -Wextra")

# Build-type specific flags. Change as needed.
SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
SET(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

message(STATUS "Building with the following extra flags: ${CMAKE_CXX_FLAGS}")

# --------------------------------------------------------------------------------
#                         Locate files (no change needed).
# --------------------------------------------------------------------------------
# We make sure that CMake sees all the files.
include_directories(
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/tests)
include_directories(SYSTEM
    ${PROJECT_SOURCE_DIR}/external/googletest/googletest
    ${PROJECT_SOURCE_DIR}/external/googletest/googletest/include)

# Make variables referring to all the sources and test files.
set(HEADERS
        include/list.h
        include/listConfig.h.in)
set(SOURCES
        src/list.c)
set(TESTFILES
        tests/ListTestTypes.cpp
        tests/list.cpp
        tests/iterator.cpp
        tests/main.cpp)
set(TEST_MAIN
        unit_tests.x)



# --------------------------------------------------------------------------------
#                            Build! (Change as needed)
# --------------------------------------------------------------------------------
# Compile all sources into a library. Called engine here (change if you wish).
add_library( engine ${SOURCES} ${HEADERS})



# --------------------------------------------------------------------------------
#                         Make Tests (no change needed).
# --------------------------------------------------------------------------------
# Add a make target 'gtest', that runs the tests (and builds all dependencies).
# The setup of Google Test is done at the very end of this file.
add_executable(${TEST_MAIN} ${TESTFILES})
add_dependencies(${TEST_MAIN} googletest engine)
target_link_libraries(${TEST_MAIN} googletest engine pthread)
add_custom_target(gtest
    COMMAND "${PROJECT_BINARY_DIR}/${TEST_MAIN}"
    DEPENDS engine ${TEST_MAIN})


# Add a standard make target 'test' that runs the tests under CTest (only as an alt. to gtest).
include(CTest)
enable_testing()
add_test(unit_tests ${PROJECT_BINARY_DIR}/${TEST_MAIN})


# --------------------------------------------------------------------------------
#                         Google Test (no change needed).
# --------------------------------------------------------------------------------
# The following makes sure that an up-to-date version of googletest is available,
# and built so that it may be used by your tests.
add_custom_target( git_update
    COMMAND git submodule init
    COMMAND git submodule update
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} )
add_library( googletest
    ${PROJECT_SOURCE_DIR}/external/googletest/googletest/src/gtest-all.cc
    ${PROJECT_SOURCE_DIR}/external/googletest/googletest/src/gtest_main.cc )
add_dependencies(googletest git_update)
set_source_files_properties(${PROJECT_SOURCE_DIR}/external/googletest/googletest/src/gtest-all.cc  PROPERTIES GENERATED 1)
set_source_files_properties(${PROJECT_SOURCE_DIR}/external/googletest/googletest/src/gtest_main.cc PROPERTIES GENERATED 1)



# --------------------------------------------------------------------------------
#                            Misc (no change needed).
# --------------------------------------------------------------------------------
# Have CMake parse the config file, generating the config header, with
# correct definitions. Here only used to make version number available to
# the source code. Include "exampleConfig.h" (no .in suffix) in the source.
configure_file (
  "${PROJECT_SOURCE_DIR}/include/listConfig.h.in"
  "${PROJECT_BINARY_DIR}/listConfig.h")
# add the binary tree to the search path for include files
# so that we will find exampleConfig.h
include_directories("${PROJECT_BINARY_DIR}")

# Ask CMake to output a compile_commands.json file for use with things like Vim YCM.
set( CMAKE_EXPORT_COMPILE_COMMANDS 1 )
