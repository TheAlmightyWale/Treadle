#pragma once
#include "TaskTraits.hpp"

namespace Treadle2
{
	//Wait synchronously for an async operation to complete
	template<IsPollable AsyncOperationType>
	void WaitSync(AsyncOperationType const& task) noexcept
	{
		while (!task.Done()) {
			task.Resume();
		}
	}

	//Wait synchronously for a set of async operations to complete
	template<IsPollable... AsyncOperationTypes>
	void WaitSync(AsyncOperationTypes const&... tasks) noexcept
	{
		bool done = false;
		while (!done) {
			done = true;
			//Resume all tasks
			((tasks.Resume(), done &= tasks.Done()), ...);
		}
	}


	//Depend on a set of async operations to complete before continuing

	//Depend on a single async operation to complete before continuing
}
