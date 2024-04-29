#pragma once
#include <thread>
#include <optional>
#include "WorkJob.h"
#include "Locks.h"

namespace Treadle
{
	class WorkStealingQueue
	{
	public:
		WorkStealingQueue(uint32_t maxJobs, std::pmr::memory_resource* pMemoryResource) noexcept;

		std::optional<Job> Pop();
		std::optional<Job> Steal();
		void Push(Job job);

	protected:
		inline bool HasJobs() const noexcept { return m_bottomIndex - m_topIndex > 0; }

		std::thread::id m_queueThreadId;
		uint32_t m_bottomIndex;
		uint32_t m_topIndex;
		std::pmr::vector<Job> m_jobs;
		Locks::SpinLock m_spinLock;
	};

	using WorkStealingQueues = std::pmr::vector<WorkStealingQueue>;
}