include(GoogleTest)
include(FetchContent)

FetchContent_Declare(
	nanobench
	GIT_REPOSITORY https://github.com/martinus/nanobench.git 
	GIT_TAG v4.3.11
	GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(nanobench)

add_executable (Benchmarks "TreadleBenchmark.cpp")
set_target_properties(Benchmarks PROPERTIES CXX_STANDARD 20 LINKER_LANGUAGE CXX)
target_link_libraries(Benchmarks PRIVATE nanobench gtest_main Treadle)

gtest_discover_tests(Benchmarks)