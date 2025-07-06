#pragma once
#include "TaskTraits.hpp"

namespace Treadle2
{
	
	//Wait synchronously for a set of async operations to complete
	template<IsPollable... AsyncOperationTypes>
	void SyncWait(AsyncOperationTypes&... tasks) noexcept
	{
		bool done = false;
		while (!done) {
			done = true;
			//Resume all tasks
			((tasks.Resume(), done &= tasks.Done()), ...);
		}
	}


	//Depend on a set of async operations to complete before continuing
	

}
