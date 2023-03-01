module;
#include <functional>
#include <stop_token>
#include <optional>

export module Worker;

import JobQueue;
import WorkJob;

namespace Treadle
{
export class Worker {
	public:
		Worker(JobQueue& queue, uint32_t id)
			: m_queue(queue)
			, m_id(id)
			, m_bIsPaused(false)
		{}

		void Run(std::stop_token stopToken)
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

		void Pause()
		{
			m_bIsPaused = true;
		}

		void Resume()
		{
			m_bIsPaused = false;
		}

	private:
		JobQueue& m_queue;
		uint32_t m_id;
		bool m_bIsPaused;
};

}
