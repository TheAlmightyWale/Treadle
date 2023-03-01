#include "JobQueue.h"
#include "WorkJob.h"
#include "Locks.h"

namespace Treadle
{
	void JobQueue::Push(Job job)
	{
		std::unique_lock<Locks::SpinLock> lock(m_spinLock);
		m_queue.emplace(job);
		if (m_bEmpty) m_bEmpty = false;
	}

	bool JobQueue::Empty() noexcept
	{
		return m_bEmpty;
	}

	std::optional<Job> JobQueue::Pop()
	{
		std::unique_lock<Locks::SpinLock> lock(m_spinLock);
		if (m_queue.empty())
		{
			if (!m_bEmpty) m_bEmpty = true;
			return std::nullopt;
		}
		else
		{
			Job job = m_queue.front();
			m_queue.pop();
			return job;
		}
	}
}