
add_library(Treadle 
				"AtomicCounter.h" "DependedTask.h" "DependedTask.cpp" "MultidependantTask.h"
				"MultidependantTask.cpp" "Task.h")
set_target_properties(Treadle PROPERTIES CXX_STANDARD 20 LINKER_LANGUAGE CXX)