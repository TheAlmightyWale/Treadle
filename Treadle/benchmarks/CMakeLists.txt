include(GoogleTest)
include(FetchContent)

FetchContent_Declare(
	nanobench
	GIT_REPOSITORY https://github.com/martinus/nanobench.git 
	GIT_TAG v4.3.11
	GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(nanobench)

add_executable (Benchmarks )
set_property(TARGET Benchmarks PROPERTY CXX_STANDARD 20)
target_link_libraries(Benchmarks PRIVATE nanobench gtest_main Treadle)

gtest_discover_tests(Benchmarks)