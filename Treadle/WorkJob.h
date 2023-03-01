#pragma once
#include <functional>

namespace Treadle
{
	class Job {
	public:
		Job(std::function<void(void)> job);
		void Execute();

	private:
		std::function<void(void)> m_fn;
	};

}