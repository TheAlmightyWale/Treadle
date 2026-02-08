#pragma once
#include "TaskTraits.hpp"
#include "Task.hpp"

namespace Treadle2
{
	namespace Detail
	{
		void DoNothing() {}

		Task<void> MakeTask()
		{
			co_return;
		}
	}

	// Wait synchronously for a set of async operations to complete
	template <IsPollable... AsyncOperationTypes>
	void SyncWait(AsyncOperationTypes &...tasks) noexcept
	{
		bool done = false;
		while (!done)
		{
			done = true;
			// Resume all tasks
			((!tasks.Done() ? tasks.Resume() : Detail::DoNothing(), done &= tasks.Done()), ...);
		}
	}

	// Depend on a set of async operations to complete before continuing
	template <IsTask... TaskTypes>
	Task<void> Wait(TaskTypes &...tasks) noexcept
	{
		// create new Task
		Task<void> t = Detail::MakeTask();
		constexpr uint32_t numTasks = sizeof...(tasks);
		t.InitializeCounter(numTasks);

		// for each task, set counter and continuation
		(tasks.SetContinuation(t.GetCoroutine()), ...);
		(tasks.SetCounter(t.GetCounter()), ...);

		// schedule tasks

		return t;
	}

}
