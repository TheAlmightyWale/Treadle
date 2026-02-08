#pragma once
#include "TaskTraits.hpp"
#include "MultiTask.hpp"

namespace Treadle2
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


	//Depend on a set of async operations to complete before continuing
	template<IsPollable... AsyncOperationTypes>
	MultiTask<void> AsyncWait(AsyncOperationTypes&... tasks) noexcept
	{
		// Create a task that we can await, which will continue once all depended tasks are done

		// we don't own the task memory we just create a task, and that will have it's own overloaded
		// allocator


	}

	

}
