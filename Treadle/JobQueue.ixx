module;
#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>

export module JobQueue;

import WorkJob;

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
		std::mutex m_mutex;
		std::atomic_bool m_bEmpty;
	};

	void JobQueue::Push(Job job)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_queue.emplace(job);
		if (m_bEmpty) m_bEmpty = false;
	}

	bool JobQueue::Empty()
	{
		return m_bEmpty;
	}

	std::optional<Job> JobQueue::Pop()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
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