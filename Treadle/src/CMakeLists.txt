
add_library(Treadle 
				"AtomicCounter.h" "Task.hpp" "TaskTraits.hpp" "JobSystem.h" "MpmcQueue.hpp" "ThreadUtils.h" "OsDef.h" "JobSystem.cpp" "TaskPool.h" "Wait.hpp")

set_target_properties(Treadle PROPERTIES CXX_STANDARD 20 LINKER_LANGUAGE CXX)