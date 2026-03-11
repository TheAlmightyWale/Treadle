add_library(${TREADLE_LIBRARY_NAME})
target_sources(${TREADLE_LIBRARY_NAME}
	PRIVATE 
		"include/Treadle/private/WaitUtils.h" "WaitUtils.cpp" "JobSystem.cpp"
 	PUBLIC
		FILE_SET HEADERS
		BASE_DIRS "include"
)

set_target_properties(${TREADLE_LIBRARY_NAME} PROPERTIES CXX_STANDARD 20 LINKER_LANGUAGE CXX)