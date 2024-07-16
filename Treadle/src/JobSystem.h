#pragma once
#include <vector>
#include <cstdint>
#include <unordered_map>
#include "MpmcQueue.hpp"
#include "Task.h"
#include "BasicPromise.h"
#include "ThreadSafeMap.hpp"

namespace Treadle
{
	class JobSystem {
	public:
		using TaskType = Task<BasicPromise>;
		using TaskIdType = int;
		using MemoryType = ThreadSafeMap<TaskIdType, TaskType>;

		//can initialize with a number of threads
		JobSystem(uint32_t numThreads);

		//delayed start to allow for testing and bench marking
		void Start() noexcept;
		void JoinAll() noexcept;

		void AddJob(TaskType&& task) noexcept;
		void EraseJob(TaskIdType id) noexcept;

		MpmcQueue<TaskIdType> const& GetQueue();

		//on destruct join threads
		~JobSystem() noexcept;

	private:
		std::vector<std::unique_ptr<std::jthread>> m_threads;
		uint32_t const m_numThreads;
		MpmcQueue<TaskIdType> m_queue;
		MemoryType m_memory;

		mutable std::mutex m_mutex;
	};
}

