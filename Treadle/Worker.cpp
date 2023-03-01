#include <optional>
#include "JobQueue.h"
#include "WorkJob.h"
#include "Worker.h"

namespace Treadle
{
	Worker::Worker(JobQueue& queue, uint32_t id)
		: m_queue(queue)
		, m_id(id)
		, m_bIsPaused(false)
	{}

	void Worker::Run(std::stop_token stopToken)
	{
		while (!stopToken.stop_requested())
		{
			if (!m_bIsPaused)
			{
				if (std::optional<Job> job = m_queue.Pop())
				{
					(*job).Execute();
				}
			}
		}
	}

	void Worker::Pause()
	{
		m_bIsPaused = true;
	}

	void Worker::Resume()
	{
		m_bIsPaused = false;
	}
}

