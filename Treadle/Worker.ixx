#include <functional>
#include <stop_token>
#include <iostream>
#include <syncstream>

export module Worker;

import JobQueue;
import WorkJob;

namespace Treadle
{
export class Worker {
	public:
		Worker(JobQueue& queue, uint32_t id);

		void Run(std::stop_token stopToken);

	private:
		JobQueue& m_queue;
		uint32_t m_id;
};

Worker::Worker(JobQueue& queue, uint32_t id)
	: m_queue(queue)
	, m_id(id)
{}

void Worker::Run(std::stop_token stopToken)
{
	std::osyncstream{ std::cout } << "Running worker: " << m_id << std::endl;
	while (!stopToken.stop_requested())
	{

		if (std::optional<Job> job = m_queue.Pop())
		{
			std::osyncstream{ std::cout } << "Worker " << m_id << " Executing job" << std::endl;
			(*job).Execute();
		}
	}
}

}
