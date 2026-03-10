add_library(${LIBRARY_NAME})
target_sources(${LIBRARY_NAME}
	PRIVATE 
		"WaitUtils.h" "WaitUtils.cpp" "JobSystem.cpp"
 	PUBLIC
		FILE_SET HEADERS
		FILES
			"AtomicCounter.h" "Task.hpp" "TaskTraits.hpp" "JobSystem.h" 
			"MpmcQueue.hpp" "ThreadUtils.h" "OsDef.h"
			"TaskPool.h" "Wait.hpp" 
)

set_target_properties(${LIBRARY_NAME} PROPERTIES CXX_STANDARD 20 LINKER_LANGUAGE CXX)

install(TARGETS ${LIBRARY_NAME}
		EXPORT ${LIBRARY_NAME}_targets
		FILE_SET HEADERS
)

include(GNUInstallDirs)

install(EXPORT ${LIBRARY_NAME}_targets
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
		NAMESPACE ${LIBRARY_NAME}::
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}ConfigVersion.cmake
  COMPATIBILITY ExactVersion
)

install(
  FILES
    cmake/${LIBRARY_NAME}Config.cmake
	${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}ConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
)