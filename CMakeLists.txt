# CMakeList.txt : CMake project for Treadle, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.25)

option(TREADLE_BUILD_RUN_TESTS "Build and run Treadle tests" FALSE)
option(TREADLE_BUILD_RUN_BENCHMARKS "Build and run Treadle benchmarks" FALSE)

function(fetch_gtest)
	include(FetchContent)
	FetchContent_Declare(
		googletest
		GIT_REPOSITORY https://github.com/google/googletest.git
  		GIT_TAG v1.17.0 
	)

	# For Windows: Prevent overriding the parent project's compiler/linker settings
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	FetchContent_MakeAvailable(googletest)
endfunction()

# Enable Hot Reload for MSVC compilers if supported.
if (MSVC AND POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Treadle" VERSION 0.1 DESCRIPTION "C++20 cooperative threading job system" LANGUAGES CXX)

# Add source to this project's executable.
set(TREADLE_LIBRARY_NAME "Treadle")
add_subdirectory("src/")

if (TREADLE_BUILD_RUN_TESTS)
	fetch_gtest()
	enable_testing()
    add_subdirectory(tests)
endif()

if(TREADLE_BUILD_RUN_BENCHMARKS)
	fetch_gtest()
	enable_testing()
	add_subdirectory(benchmarks)
endif()

install(TARGETS ${TREADLE_LIBRARY_NAME}
		EXPORT treadle-targets
		FILE_SET HEADERS
		DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${TREADLE_LIBRARY_NAME}
)

include(GNUInstallDirs)

install(EXPORT treadle-targets
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${TREADLE_LIBRARY_NAME}
		NAMESPACE Treadle::
)

include(CMakePackageConfigHelpers)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/TreadleConfig.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Treadle
)

write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/TreadleConfigVersion.cmake"
	COMPATIBILITY ExactVersion
)

install(FILES
          "${CMAKE_CURRENT_BINARY_DIR}/TreadleConfig.cmake"
          "${CMAKE_CURRENT_BINARY_DIR}/TreadleConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Treadle
)