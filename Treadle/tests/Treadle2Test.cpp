#include "gtest/gtest.h"
#include "TestUtils.hpp"
#include <Task.hpp>

#include <vector>

namespace Treadle2 {

	Task<void> DoThing() {
		co_return;
	}

	Task<int> GetNum() {
		co_return 1;
	}

	Task<int> CalcSum() {
		int sum = 1;

		sum += 1;
		co_await DoThing();

		sum += 1;
		co_return sum;
	}

	TEST(TaskTests, InitialzedAsPaused) {
		auto task = DoThing();
		EXPECT_FALSE(task.Done());
	}

	TEST(TaskTests, ReturnNull) {
		auto task = DoThing();
		task.Resume();

		EXPECT_TRUE(task.Done());
	}

	TEST(TaskTests, ReturnSome) {
		auto task = GetNum();
		task.Resume();

		EXPECT_TRUE(task.Done());
		EXPECT_EQ(task.Value(), 1);
	}

	TEST(TaskTests, callContinuationOnCompletionSingleDependency) {
		//set up continuation that sets a value to true after awaiting another task
		FlagSetter setter;
		Event event;
		auto dependedTask = WaitOnEvent(event);
		auto task = setter.WaitAndSet(dependedTask);
		task.Resume();
		EXPECT_FALSE(setter.GetFlag());

		//depended Task should finish and then immediately continue task and set it's flag
		dependedTask.Resume();

		//assert that variable was set to true
		EXPECT_TRUE(setter.GetFlag());
	}


	//TEST(Task, RaiseError) {
	//
	//}

	//TEST(JobSchedulerTests, InsertedTaskAddsDependency) {

	//}

	//TEST(JobSchedulerTests, ProcessTasksInDesiredOrder) {

	//}

	//TEST(JobSchedulerTests, InterruptsLongRunningTasks) {

	//}


}

