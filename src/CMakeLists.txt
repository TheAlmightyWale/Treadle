add_library(${TREADLE_LIBRARY_NAME})
target_sources(${TREADLE_LIBRARY_NAME}
	PRIVATE 
		"WaitUtils.h" "WaitUtils.cpp" "JobSystem.cpp"
 	PUBLIC
		FILE_SET HEADERS
		FILES
			"AtomicCounter.h" "Task.hpp" "TaskTraits.hpp" "JobSystem.h" 
			"MpmcQueue.hpp" "ThreadUtils.h" "OsDef.h"
			"TaskPool.h" "Wait.hpp" 
)

set_target_properties(${TREADLE_LIBRARY_NAME} PROPERTIES CXX_STANDARD 20 LINKER_LANGUAGE CXX)