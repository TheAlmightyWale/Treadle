#pragma once
#include <coroutine>
#include "AtomicCounter.h"

namespace Treadle
{
	struct DependedTask {
		struct FinalizeTask;
		struct promise_type;
		struct Awaiter;

		Awaiter operator co_await() const&& noexcept;

		void Resume() const noexcept;
		bool Done() const noexcept;

		~DependedTask();
		DependedTask& operator=(DependedTask&&) noexcept;
		DependedTask(DependedTask&&) noexcept;

		DependedTask& operator=(DependedTask&) const = delete;
		DependedTask(DependedTask&) = delete;

	private:
		explicit DependedTask(std::coroutine_handle<promise_type> h) noexcept;
		void Swap_(DependedTask& rhs) noexcept;

		std::coroutine_handle<promise_type> coro_;
	};

	struct DependedTask::Awaiter {
		explicit Awaiter(std::coroutine_handle<promise_type> coro);

		bool await_ready() const noexcept;
		std::coroutine_handle<> await_suspend(std::coroutine_handle<> h) const noexcept;
		void await_resume() const noexcept;

		std::coroutine_handle<promise_type> coro_;
	};

	struct DependedTask::FinalizeTask {
		bool await_ready() const noexcept;
		void await_resume() const noexcept;
		std::coroutine_handle<> await_suspend(std::coroutine_handle<DependedTask::promise_type> h) const noexcept;
	};

	struct DependedTask::promise_type {
		DependedTask get_return_object() noexcept;
		std::suspend_always initial_suspend() const noexcept;
		FinalizeTask final_suspend() const noexcept;
		void return_void() const noexcept;
		void unhandled_exception() const  noexcept {
			//should terminate as marked noexcept
		}

		promise_type(Counter_t* pCounter, std::coroutine_handle<>* pContinuation) noexcept
			: pDependedTasksRemaining(pCounter)
			, dependentCoro(pContinuation)
		{}

		//Need to guarantee that the counter this points to is always in scope
		Counter_t* pDependedTasksRemaining;
		//The multidependentTask to resume once all BasicTasks are complete
		std::coroutine_handle<>* dependentCoro;
	};
}