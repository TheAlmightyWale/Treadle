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

	private:
		std::queue<Job> m_queue;
		std::mutex m_mutex;
	};

	void JobQueue::Push(Job job)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_queue.emplace(job);
	}

	std::optional<Job> JobQueue::Pop()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		if (m_queue.empty())
		{
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