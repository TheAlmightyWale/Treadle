
add_library(Treadle 
				"AtomicCounter.h" "DependedTask.h" "DependedTask.cpp" "MultidependentTask.h"
				"MultidependentTask.cpp" "Task.h" "JobSystem.h" "MpmcQueue.hpp" "ThreadUtils.h" "OsDef.h" "JobSystem.cpp"     "TaskPool.h" "BasicPromise.h")

set_target_properties(Treadle PROPERTIES CXX_STANDARD 20 LINKER_LANGUAGE CXX)