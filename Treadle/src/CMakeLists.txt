add_library(${LIBRARY_NAME} 
				"AtomicCounter.h" "Task.hpp" "TaskTraits.hpp" "JobSystem.h" 
				"MpmcQueue.hpp" "ThreadUtils.h" "OsDef.h" "JobSystem.cpp"
				 "TaskPool.h" "Wait.hpp" "WaitUtils.cpp")

set_target_properties(${LIBRARY_NAME} PROPERTIES CXX_STANDARD 20 LINKER_LANGUAGE CXX)

target_include_directories(${LIBRARY_NAME} PUBLIC 
							"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
                           	"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")