#include <cstdint>
#include <vector>
#include <thread>
#include <functional>
#include <chrono>

export module JobSystem;

import WorkJob;
import Worker;
import JobQueue;


namespace Treadle
{

export class JobSystem
{
public:
	JobSystem(uint32_t numThreads);
	JobSystem(JobSystem const&) = delete;
	JobSystem& operator=(JobSystem const&) = delete;

	JobSystem(JobSystem const&& other) = delete;
	JobSystem& operator=(JobSystem const&& other) = delete;

	~JobSystem();

	JobQueue& GetQueue();

private:
	JobQueue m_jobs;
	std::vector<std::thread> m_threads;
	std::stop_source m_stopSource;
};

JobSystem::JobSystem(uint32_t numThreads)
	: m_stopSource()
	, m_jobs()
	, m_threads()
{
	for (uint32_t i = 0; i < numThreads; ++i)
	{
		Worker worker(m_jobs, i);
		auto workFunction = std::bind(&Worker::Run, worker, m_stopSource.get_token());
		m_threads.emplace_back(workFunction);
	}
}

JobQueue& JobSystem::GetQueue()
{
	return m_jobs;
}

JobSystem::~JobSystem()
{
	//wait 3 seconds cause
	using namespace std::literals::chrono_literals;
	std::this_thread::sleep_for(3s);

	//Send stop signal to workers
	m_stopSource.request_stop();

	for (std::thread& thread : m_threads)
	{
		thread.join();
	}
}
}