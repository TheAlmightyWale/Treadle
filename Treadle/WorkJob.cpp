#include "WorkJob.h"

namespace Treadle
{
	Job::Job(std::function<void(void)> job)
		: m_fn(job)
	{}

	void Job::Execute()
	{
		m_fn();
	}
}
