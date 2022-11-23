#include <cstdint>
#include <vector>
#include <thread>
#include <functional>
#include <chrono>
#include <thread>

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

	void Stop();

	JobQueue& GetQueue();

private:
	std::vector<std::thread> m_threads;
	JobQueue m_jobs;
	std::vector<Worker> m_workers;
	std::stop_source m_stopSource;
};

JobSystem::JobSystem(uint32_t numThreads)
	: m_stopSource()
	, m_threads()
	, m_jobs(m_stopSource.get_token())
	, m_workers()
{
	for (uint32_t i = 0; i < numThreads; ++i)
	{
		m_workers.emplace_back(m_jobs, i);
		auto workFunction = std::bind(&Worker::Run, std::ref(m_workers[i]), m_stopSource.get_token());
		m_threads.emplace_back(workFunction);
	}
}

JobQueue& JobSystem::GetQueue()
{
	return m_jobs;
}

void JobSystem::Stop()
{
	//wait 3 seconds cause
	using namespace std::literals::chrono_literals;
	std::this_thread::sleep_for(3s);

	//Send stop signal to workers, this will also wake them up if they are idling
	m_stopSource.request_stop();
	
	for (std::thread& thread : m_threads)
	{
		thread.join();
	}
}
}