#include <cassert>
#include "OsIncludes.h"
#include "JobSystem.h"

namespace Treadle
{
	JobSystem::JobSystem(uint32_t numThreads)
		: m_stopSource()
		, m_jobs()
		, m_threads()
	{
		assert(numThreads <= 64 /*Cannot handle more than 64 threads currently*/);

		for (uint32_t i = 0; i < numThreads; ++i)
		{
			m_workers.push_back(std::make_unique<Worker>(m_jobs, i));
			auto workFunction = std::bind(&Worker::Run, m_workers[i].get(), m_stopSource.get_token());
			auto& newThread = m_threads.emplace_back(workFunction);
			SetWorkerThreadAffinity(1i64 << i, newThread);
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

	void JobSystem::SetWorkerThreadAffinity(std::bitset<64> threadAffinityBitMask, std::thread& thread)
	{
#ifdef _WIN64
		DWORD_PTR affinityMask = threadAffinityBitMask.to_ulong();
		DWORD_PTR res = SetThreadAffinityMask(thread.native_handle(), affinityMask);
		assert(res > 0 /* "Failed to set thread affinity mask" */);
#else
		static_assert(false, "Implmentation for other OS' required");
#endif 

	}

	//Can potentially throw
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