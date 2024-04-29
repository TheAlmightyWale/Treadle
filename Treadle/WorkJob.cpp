#include "WorkJob.h"

namespace Treadle
{
	Job::Job(std::function<void(void)> job) noexcept
		: m_fn{ job }
	{}

	Job::Job() noexcept
		:m_fn{nullptr}
	{}

	void Job::Execute()
	{
		if(m_fn) m_fn();
	}
}
