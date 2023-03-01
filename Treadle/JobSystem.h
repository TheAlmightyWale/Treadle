#pragma once
#include <cstdint>
#include <bitset>
#include <vector>
#include <stop_token>
#include <thread>
#include <memory>
#include "JobQueue.h"
#include "Worker.h"

//TODO: future improvements
// work stealing - implementation and benchmarking
// dependant job chains
// Queues accept: co-routines, lambdas and functors with variable arguments 
// Returning / retrieving completed results from work??
namespace Treadle
{

class JobSystem
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

	void SetWorkerThreadAffinity(std::bitset<64> threadAffinityBitMask, std::thread& thread);

	JobQueue m_jobs;
	std::vector<std::thread> m_threads;
	std::vector<std::unique_ptr<Worker>> m_workers;
	std::stop_source m_stopSource;
};

}