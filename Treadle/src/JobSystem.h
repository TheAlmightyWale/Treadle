#pragma once
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <coroutine>
#include <utility>
#include "MpmcQueue.hpp"
#include "TaskTraits.hpp"

namespace Treadle
{
	// Type Erased version of Task
	// JobSystem should not care about return values of Task, just about kicking it off
	class Job
	{
	public:
		template <IsTask T>
		Job(T const& task) noexcept
			: coro_(task.GetCoroutine())
		{}

		Job& operator=(Job const&) = delete;
		Job(Job const&) = delete;

		Job& operator=(Job&& other) noexcept
		{
			std::swap(coro_, other.coro_);
			return *this;
		}

		Job(Job const&& other) noexcept
		: coro_(other.coro_)
		{}

		void Start() { coro_.resume(); }

	//private:
		std::coroutine_handle<> coro_;
	};

	class JobSystem
	{
	public:
		// can initialize with a number of threads
		JobSystem(uint32_t numThreads);

		// delayed start to allow for testing and bench marking
		void Start() noexcept;
		void JoinAll() noexcept;

		void AddJob(Job &&job) noexcept;

		MpmcQueue<Job> const& GetQueue();

		// on destruct join threads
		~JobSystem() noexcept;

	private:
		std::vector<std::unique_ptr<std::jthread>> m_threads;
		uint32_t const m_numThreads;
		MpmcQueue<Job> m_queue;
	};
}
