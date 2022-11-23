#include <queue>
#include <mutex>
#include <condition_variable>

export module JobQueue;

import WorkJob;

namespace Treadle
{

	export class JobQueue
	{
	public:
		JobQueue(std::stop_token stopToken);

		Job Pop();
		void Push(Job job);

	private:
		std::queue<Job> m_queue;
		std::mutex m_mutex;
		std::condition_variable_any m_waitOnNonEmptyQueue;
		std::stop_token m_jobSystemStoppedInterrupt;
	};

	JobQueue::JobQueue(std::stop_token stopToken)
		: m_queue()
		, m_mutex()
		, m_waitOnNonEmptyQueue()
		, m_jobSystemStoppedInterrupt(stopToken)
	{}

	void JobQueue::Push(Job job)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_queue.emplace(job);
		m_waitOnNonEmptyQueue.notify_one();
	}

	Job JobQueue::Pop()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_waitOnNonEmptyQueue.wait(
			lock,
			m_jobSystemStoppedInterrupt,
			[this] {return this->m_queue.empty(); });

		Job job = m_queue.front();
		m_queue.pop();
		return job;
	}
}