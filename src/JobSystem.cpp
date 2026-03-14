#include "Treadle/public/JobSystem.h"
#include <cstdint>
#include <unordered_map>
#include <string>
#include "Treadle/public/ThreadUtils.h"
#include "Treadle/public/MpmcQueue.hpp"
#include "Treadle/public/Task.hpp"

namespace
{
	void Work(std::stop_token stop, Treadle::MpmcQueue<Treadle::Job> &workQueue)
	{
		// pull from queue or spin waiting for something to be added to it
		while (!stop.stop_requested())
		{
			auto oCoro = workQueue.try_pop();
			if (oCoro)
			{
				auto& coro = *oCoro;
				// right now we just start once and assume task runs to completion
				coro.resume();
			}
		}
	}
}

namespace Treadle
{
	JobSystem::JobSystem(uint32_t numThreads)
		: m_numThreads(numThreads)
	{
		m_threads.reserve(numThreads);
	}

	void JobSystem::Start() noexcept
	{
		for (uint32_t i = 0; i < m_numThreads; ++i)
		{
			m_threads.emplace_back(CreateAndStartThread(i, "Worker Thread-" + std::to_string(i), Work, m_queue));
		}
	}

	void JobSystem::JoinAll() noexcept
	{
		for (auto &pThread : m_threads)
		{
			if (pThread->joinable())
			{
				pThread->get_stop_source().request_stop();
				pThread->join();
			}
		}
	}

	void JobSystem::AddJob(Job const& job) noexcept
	{
		m_queue.push(job);
	}

	MpmcQueue<Job> const& JobSystem::GetQueue()
	{
		return m_queue;
	}

	JobSystem::~JobSystem() noexcept
	{
		JoinAll();
	}
}
