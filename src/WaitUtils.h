#pragma once
#include "Task.hpp"

namespace Treadle
{
	namespace Detail
	{
		void DoNothing();

		Task<void> MakeTask();
	}
}
