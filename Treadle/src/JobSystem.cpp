#include "JobSystem.h"
#include <cstdint>
#include <unordered_map>
#include <string>
#include "ThreadUtils.h"
#include "MpmcQueue.hpp"
#include "Task.hpp"

namespace
{
	void Work(std::stop_token stop, Treadle::MpmcQueue<Treadle::Job> &workQueue)
	{
		// pull from queue or spin waiting for something to be added to it
		while (!stop.stop_requested())
		{
			auto oTask = workQueue.try_pop();
			if (oTask)
			{
				auto& task = *oTask;
				// right now we just start once and assume task runs to completion
				task.Start();
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

	void JobSystem::AddJob(Job&& task) noexcept
	{
		m_queue.push(std::move(task));
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
