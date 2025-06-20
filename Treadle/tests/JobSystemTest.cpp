//#include "gtest/gtest.h"
//#include "../src/JobSystem.h"
//#include "../src/DependedTask.h"
//
//TEST(JobSystem, CleansUpTaskWhenCompleted) {
//	//Create system
//
//	//give it a task
//
//	//check if task memory was cleared
//}
//
//int GetId()
//{
//	static int id;
//	id++;
//	return id;
//}
//
////adds task id to queue on final_suspend
//struct TrackedTestPromise {
//	TrackedTestPromise(std::queue<int>& startQueue,std::queue<int>& completionQueue)
//		:completionQueue(completionQueue)
//		, startQueue(startQueue)
//		, id(GetId())
//	{}
//
//	Treadle::Task<TrackedTestPromise> get_return_object() noexcept {
//		return Treadle::Task<TrackedTestPromise>{ std::coroutine_handle<TrackedTestPromise>::from_promise(*this) };
//	}
//
//	std::suspend_always initial_suspend() const noexcept {
//		startQueue.emplace(id);
//		return {};
//	}
//
//	std::suspend_always final_suspend() const noexcept {
//		completionQueue.emplace(id);
//		return {};
//	}
//
//	void unhandled_exception() const noexcept {}
//
//	void return_void() const noexcept {}
//
//private:
//	std::queue<int>& completionQueue;
//	std::queue<int>& startQueue;
//	int id;
//};
//
//Treadle::Task<TrackedTestPromise> CreateQueuedTask(std::queue<int>& startQueue, std::queue<int>& completionQueue) {
//	co_return;
//}
//
//Treadle::DependedTask<TrackedTestPromise> CreateDependedTask() {
//
//}
//
//TEST(JobSystem, MaintainsDependencyOrders) {
//	std::queue<int> taskCompletionOrder;
//	std::queue<int> taskInitiationOrder;
//	
//	//Create system, with a single thread
//	Treadle::JobSystem js(1);
//
//	//tasks have an id and we put that id into a queue on completion
//	//Create one set of dependent tasks
//	auto task1 = CreateQueuedTask(taskInitiationOrder, taskCompletionOrder);
//	auto task2 = CreateDependedTask(taskInitiationOrder, taskCompletionOrder, task1);
//	//create other independent tasks
//	auto task3 = CreateQueuedTask(taskInitiationOrder, taskCompletionOrder);
//	auto task4 = CreateQueuedTask(taskInitiationOrder, taskCompletionOrder);
//
//	//run system
//	js.Start();
//
//	js.JoinAll();
//
//	//check ordering of jobs run
//	//find task1 id
//	//check task2 was completed immediately afterwards
//}