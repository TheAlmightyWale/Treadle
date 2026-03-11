#pragma once
#include "Treadle/public/Task.hpp"

namespace Treadle
{
	namespace Detail
	{
		void DoNothing();

		Task<void> MakeTask();
	}
}
