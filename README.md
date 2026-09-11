# Collection of GoogleTest examples

Example of setup of gtest with C sources

* Can test C and C++ sources
* Tests are written in C++
* No requirement for a `main()` in the unit tests.
* Uses CMake so easy to integrate into `CMake` projects
* Tests can be `Asserts` or `Expects` the first will bomb out on failure the
second marks the failure but continues with the rest of the tests.
* When testing some C data types sometimes extra steps are needed to convert to
C++ types.

## Examples

* Fixtures
* Mocks
* Algorithms
* Checking floating point numbers with an acceptable error
* Mocking out something that calls a HAL
* Functions that require setup in advance
* Death tests: core dumps and segmentation faults

## Dependencies

* Docker Engine - [script to automatically install and setup](https://github.com/parrotrueper/docker-install)
* Docker Compose - [script to automatically install and setup](https://github.com/parrotrueper/install-docker-compose)

## Build and run the test suite

`ci/test`

## Expected results from a test run

* All tests should pass
