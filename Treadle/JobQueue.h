#pragma once
#include <queue>
#include <condition_variable>
#include <optional>
#include "WorkJob.h"
#include "Locks.h"

namespace Treadle
{
	class JobQueue
	{
	public:
		JobQueue() noexcept = default;

		std::optional<Job> Pop();
		void Push(Job job);
		bool Empty() noexcept;

	private:
		std::queue<Job> m_queue;
		Locks::SpinLock m_spinLock;
		std::atomic_bool m_bEmpty;
	};

}