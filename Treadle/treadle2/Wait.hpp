#pragma once
#include "TaskTraits.hpp"

namespace Treadle2
{

	void DoNothing() {}
	
	//Wait synchronously for a set of async operations to complete
	template<IsPollable... AsyncOperationTypes>
	void SyncWait(AsyncOperationTypes&... tasks) noexcept
	{
		bool done = false;
		while (!done) {
			done = true;
			//Resume all tasks
			((!tasks.Done()? tasks.Resume() : DoNothing(), done &= tasks.Done()), ...);
		}
	}


	//Depend on a set of async operations to complete before continuing
	template<class ReturnType, IsPollable... AsyncOperationTypes>
	Task<ReturnType> AsyncWait(AsyncOperationTypes&... tasks) noexcept
	{
		// Create a task that we can await, which will continue once all depended tasks are done

	}

	

}
