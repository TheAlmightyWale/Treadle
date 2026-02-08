set(TARGET_NAME Treadle2)

add_library(${TARGET_NAME} INTERFACE)

target_include_directories(${TARGET_NAME} INTERFACE "Task.hpp" "Wait.hpp" "TaskTraits.hpp")

set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 20 LINKER_LANGUAGE CXX)