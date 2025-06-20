#include "gtest/gtest.h"
#include "../treadle2/Task.hpp"
#include "../treadle2/Wait.hpp"

#include <vector>

namespace Treadle2 {
	//TEST(SplitTask, splitsAndRuns) {

	//}

	//Task<DependedPromise> CreateDependedTask(Counter_t& counter, std::coroutine_handle<>& continuation) noexcept {
	//	co_return;
	//}

	//Task<Promise> CompleteTest(Counter_t const& checkedCounter) noexcept {
	//	EXPECT_EQ(checkedCounter, 0);
	//	co_return;
	//}

	//TEST(SharedCounter, InitializeWithMultipleTasks) {
	//	Counter_t counter;
	//	auto finishingTask = CompleteTest(counter);

	//	std::vector <Task<DependedPromise>> tasks;
	//	for (int i = 0; i < 5; i++) {
	//		tasks.emplace_back(CreateDependedTask(counter, finishingTask));
	//	}
	//}

	//TEST(SharedCounter, DecrementsOnCompletion) {

	//}

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

	TEST(Task, ReturnSome) {
		auto task = GetNum();
		task.Resume();

		EXPECT_TRUE(task.Done());
		EXPECT_EQ(task.Value(), 1);
	}

	TEST(Task, PauseThenReturn) {
		auto task = CalcSum();
		task.Resume();

		EXPECT_FALSE(task.Done());

		while (!task.Done())
		{
			task.Resume();
		}
		EXPECT_EQ(task.Value(), 3);
	}

	bool bSet = false;
	Task<void> WaitAndSet()
	{
		co_await DoThing();
		bSet = true;
	}

	TEST(Task, callContinuationOnCompletion) {
		//set up continuation that sets a value to true after awaiting another task
		auto task = WaitAndSet();
		task.Resume();
		EXPECT_FALSE(bSet);

		//TODO have not chained anything at this point, we need to add another
		//task that we complete to then see if initial task continues automatically

		//assert that variable was set to true
		EXPECT_TRUE(bSet);
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

	//TEST(SyncWaitTests, WaitOne) {

	//}

	//TEST(SyncWaitTests, WaitMany) {

	//}
}

