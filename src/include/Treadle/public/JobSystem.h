#pragma once
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <coroutine>
#include <utility>
#include "MpmcQueue.hpp"
#include "TaskTraits.hpp"
#include "Job.h"

namespace Treadle
{
	class JobSystem
	{
	public:
		// can initialize with a number of threads
		JobSystem(uint32_t numThreads);

		// delayed start to allow for testing and bench marking
		void Start() noexcept;
		void JoinAll() noexcept;

		void AddJob(Job const& job) noexcept;

		MpmcQueue<Job> const& GetQueue();

		// on destruct join threads
		~JobSystem() noexcept;

	private:
		std::vector<std::unique_ptr<std::jthread>> m_threads;
		uint32_t const m_numThreads;
		MpmcQueue<Job> m_queue;
	};
}
