#pragma once
#include <coroutine>
#include <atomic>
#include <memory>

namespace Treadle
{
	using CounterType = uint32_t;
	using Counter_t = std::atomic<uint32_t>;

	//Takes in some set of tasks and returns a single awaitable 
	// that will have a counter atached to all of the tasks given to it
	struct MultiDependentTask {
		struct FinalizeTask;
		struct promise_type;
		struct Awaiter;

		Counter_t counter_;
		std::coroutine_handle<promise_type> coro_;

		Awaiter operator co_await() const && noexcept;

		void return_void() const {}

		void Resume() const noexcept;
		bool Done() const noexcept;

		~MultiDependentTask();

		MultiDependentTask& operator=(MultiDependentTask&&) noexcept;
		MultiDependentTask(MultiDependentTask&&) noexcept;

		MultiDependentTask& operator=(MultiDependentTask&) const = delete;
		MultiDependentTask(MultiDependentTask&) = delete;

	private:
		explicit MultiDependentTask(std::coroutine_handle<promise_type> h) noexcept;
		void Swap_(MultiDependentTask& rhs) noexcept;
	};

	struct MultiDependentTask::Awaiter {
		explicit Awaiter(std::coroutine_handle<promise_type> currentCoro) : coro_(currentCoro)
		{}

		bool await_ready() const noexcept;

		std::coroutine_handle<> await_suspend(std::coroutine_handle<> h) const noexcept;

		void await_resume() const noexcept {}

		std::coroutine_handle<promise_type> coro_;
	};

	struct MultiDependentTask::promise_type {
		
		//this wait count should always be valid as it is tied to the coroutine associated with this promise
		Counter_t& waitCount_;
		//the coro that is depends on these set of tasks to be completed
		std::coroutine_handle<> dependent_;

		MultiDependentTask get_return_object() noexcept {
			return MultiDependentTask{ std::coroutine_handle<MultiDependentTask::promise_type>::from_promise(*this) };
		}

		std::suspend_always initial_suspend() const { return {}; }
	};

	struct MultiDependentTask::FinalizeTask {
		bool await_ready() const noexcept {
			return false;
		}

		void await_resume() const noexcept {}

		std::coroutine_handle<> await_suspend(std::coroutine_handle<MultiDependentTask::promise_type> h) const noexcept;
	};
}