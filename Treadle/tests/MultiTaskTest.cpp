#include "gtest/gtest.h"
#include "TestUtils.hpp"

#include <MultiTask.hpp>

namespace Treadle2
{
	TEST(MultiTaskTests, InitialzedAsPaused) {
		//MultiTask itself should initialize as paused
		// as well as the tasks it is waiting on so we can correctly set it's counter
	}

	TEST(MultiTaskTests, ReturnNull) {
		
	}

	TEST(MultiTaskTests, ReturnSome) {
		
	}

	//create multiple single tasks,
	// use async wait to get a multitask
	// sync wait just the multitask
	TEST(MultiTaskTests, WaitMany)
	{

	}

	// variations where tasks are already completed

	// some complete, some not

	//do we need to do anything with lifetime guarantees?
}