#include "JobSystem.h"
#include <cstdint>
#include <unordered_map>
#include <string>
#include "ThreadUtils.h"
#include "MpmcQueue.hpp"
#include "Task.h"

namespace {
	void Work(std::stop_token stop, Treadle::MpmcQueue<Treadle::JobSystem::TaskIdType>& workQueue,  Treadle::JobSystem::MemoryType& memory) {
		//pull from queue or spin waiting for something to be added to it
		while (!stop.stop_requested()) {
			auto oTask = workQueue.try_pop();
			if (oTask) {
				auto& task = memory.at(*oTask);
				// right now we just resume once and assume task runs to completion
				// in future we will have multiple queues, where we add tasks that are ready to be resumed onto the highest priority one
				task.Resume();
				
				if (task.Done()) {
					memory.erase(task.m_id);
				}
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
		for (uint32_t i = 0; i < m_numThreads; ++i) {
			m_threads.emplace_back(CreateAndStartThread(i, "Worker Thread-" + std::to_string(i), Work, m_queue, m_memory));
		}
	}

	void JobSystem::JoinAll() noexcept
	{
		for (auto& pThread : m_threads) {
			if (pThread->joinable()) {
				pThread->get_stop_source().request_stop();
				pThread->join();
			}
		}
	}

	void JobSystem::AddJob(TaskType&& task) noexcept
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		auto [it, bNewAdded] = m_memory.emplace(task.m_id, std::forward<TaskType>(task));
		m_queue.push(it->first);
	}

	void JobSystem::EraseJob(TaskIdType id) noexcept
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_memory.erase(id);
	}

	MpmcQueue<JobSystem::TaskIdType> const& JobSystem::GetQueue()
	{
		return m_queue;
	}

	JobSystem::~JobSystem() noexcept
	{
		JoinAll();
	}
}




