#include "include/Treadle/private/WaitUtils.h"
#include <coroutine>

namespace Treadle
{
	namespace Detail
	{
        void DoNothing(){}

		Task<void> MakeTask()
		{
			co_return;
		}
	}
}