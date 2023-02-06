module;
#include <cstdint>
#include <vector>
#include <thread>
#include <functional>
#include <chrono>

//TODO: future improvements
// work stealing - implementation and benchmarking
// dependant job chains
// Queues accept: co-routines, lambdas and functors with variable arguments 
// Returning / retrieving completed results from work??

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
	void Pause();
	void Resume();

private:
	JobQueue m_jobs;
	std::vector<std::thread> m_threads;
	std::vector<std::unique_ptr<Worker>> m_workers;
	std::stop_source m_stopSource;
	//Add barrier / other pause token for pausing and resuming?
	// either that or hold on to workers and pause each of them
};

JobSystem::JobSystem(uint32_t numThreads)
	: m_stopSource()
	, m_jobs()
	, m_threads()
{
	for (uint32_t i = 0; i < numThreads; ++i)
	{
		m_workers.push_back(std::make_unique<Worker>(m_jobs, i));
		auto workFunction = std::bind(&Worker::Run, m_workers[i].get(), m_stopSource.get_token());
		m_threads.emplace_back(workFunction);
	}
}

JobQueue& JobSystem::GetQueue()
{
	return m_jobs;
}

//Prevents jobSystem from processing any more jobs, however current jobs in flight will still continue
void JobSystem::Pause()
{
	for (auto& worker : m_workers)
	{
		worker->Pause();
	}
}

//Starts the jobSystemProcessing jobs again
void JobSystem::Resume()
{
	for (auto& worker : m_workers)
	{
		worker->Resume();
	}
}

JobSystem::~JobSystem()
{
	//Wait for all jobs to complete
	while (!m_jobs.Empty()) {}

	//Send stop signal to workers
	m_stopSource.request_stop();

	for (std::thread& thread : m_threads)
	{
		thread.join();
	}
}
}