# CMakeList.txt : CMake project for Treadle, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.20)

option(BUILD_RUN_TESTS "Build and run tests" TRUE)
option(BUILD_RUN_BENCHMARKS "Build and run benchmarks" TRUE)

function(fetch_gtest)
	include(FetchContent)
	FetchContent_Declare(
		googletest
		URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
		DOWNLOAD_EXTRACT_TIMESTAMP true
	)

	# For Windows: Prevent overriding the parent project's compiler/linker settings
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	FetchContent_MakeAvailable(googletest)
endfunction()

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Treadle" VERSION 0.1 DESCRIPTION "C++20 cooperative threading job system" LANGUAGES CXX)

# Add source to this project's executable.
add_subdirectory("src/")
add_subdirectory("treadle2/")

if (BUILD_RUN_TESTS)
	fetch_gtest()
	enable_testing()
    add_subdirectory(tests)
endif()

if(BUILD_RUN_BENCHMARKS)
	fetch_gtest()
	enable_testing()
	add_subdirectory(benchmarks)
endif()