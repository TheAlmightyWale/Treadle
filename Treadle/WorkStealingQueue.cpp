#include <mutex>
#include <cassert>
#include "WorkStealingQueue.h"
#include "WorkJob.h"
#include "Locks.h"

namespace Treadle
{
	WorkStealingQueue::WorkStealingQueue(uint32_t maxJobs, std::pmr::memory_resource* const pMemoryResource) noexcept
		: m_queueThreadId{ std::this_thread::get_id() }
		, m_bottomIndex{ 0 }
		, m_topIndex{ 0 }
		, m_jobs {maxJobs, pMemoryResource}
	{}

	std::optional<Job> WorkStealingQueue::Pop()
	{
		//Pop and Push should only be called from their originating thread
		assert(std::this_thread::get_id() == m_queueThreadId);
		std::unique_lock<Locks::SpinLock> lock(m_spinLock);

		if (!HasJobs())
		{
			return std::nullopt;
		}

		m_bottomIndex--;
		return m_jobs.at(m_bottomIndex % m_jobs.size());
	}

	std::optional<Job> WorkStealingQueue::Steal()
	{
		std::unique_lock<Locks::SpinLock> lock(m_spinLock);

		if (!HasJobs())
		{
			return std::nullopt;
		}

		m_topIndex++;
		return m_jobs.at((m_topIndex - 1) % m_jobs.size());
	}

	void WorkStealingQueue::Push(Job job)
	{
		//Pop and Push should only be called from their originating thread
		assert(std::this_thread::get_id() == m_queueThreadId);
		std::unique_lock<Locks::SpinLock> lock(m_spinLock);

		m_jobs.at(m_bottomIndex % m_jobs.size()) = job;
		m_bottomIndex++;
	}

}

