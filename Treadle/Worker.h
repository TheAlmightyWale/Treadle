#pragma once
#include <cstdint>
#include <stop_token>

class JobQueue;

namespace Treadle
{
	class Worker {
	public:
		Worker(JobQueue& queue, uint32_t id);

		void Run(std::stop_token stopToken);
		void Pause();
		void Resume();

	private:
		JobQueue& m_queue;
		uint32_t m_id;
		bool m_bIsPaused;
	};
}