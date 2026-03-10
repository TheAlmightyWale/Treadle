include(GoogleTest)

set(TARGET_NAME "TreadleTest")

add_executable(${TARGET_NAME}   "JobSystemTest.cpp"  "TaskTest.cpp" "WaitTest.cpp" "TestUtils.hpp")
set_target_properties(${TARGET_NAME}  PROPERTIES CXX_STANDARD 20)
target_link_libraries(${TARGET_NAME}  PRIVATE gtest_main PUBLIC ${LIBRARY_NAME})
target_compile_options(${TARGET_NAME} PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/EHsc>)

gtest_discover_tests(${TARGET_NAME})