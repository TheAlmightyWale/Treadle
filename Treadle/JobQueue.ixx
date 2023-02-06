module;

#include <queue>
#include <condition_variable>
#include <optional>

export module JobQueue;

import WorkJob;
import Locks;

namespace Treadle
{

	export class JobQueue
	{
	public:
		JobQueue() noexcept = default;

		std::optional<Job> Pop();
		void Push(Job job);
		//Just reading this result, so does not need to be atomic
		bool Empty();

	private:
		std::queue<Job> m_queue;
		Locks::SpinLock m_spinLock;
		std::atomic_bool m_bEmpty;
	};

	void JobQueue::Push(Job job)
	{
		std::unique_lock<Locks::SpinLock> lock(m_spinLock);
		m_queue.emplace(job);
		if (m_bEmpty) m_bEmpty = false;
	}

	bool JobQueue::Empty()
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