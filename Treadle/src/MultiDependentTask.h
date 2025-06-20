#pragma once
#include <coroutine>
#include "AtomicCounter.h"
#include <memory>

namespace Treadle
{
	template<typename PromiseType>
	struct MdTask {
		using promise_type = PromiseType;

		struct FinalizeTask;
		struct Awaiter;

		//Count of tasks this task is dependent on
		Counter_t waitCount_;
		//coroutine that should be continued once this task and dependents have been completed
		//todo needed?
		std::coroutine_handle<> continuation_ = std::noop_coroutine();

		Awaiter operator co_await() & noexcept
		{
			return Awaiter{ *this };
		}

		~MdTask() {}

		MdTask& operator=(MdTask&& rhs) noexcept
		{
			if (std::addressof(rhs) != this) {
				Swap_(rhs);
			}
			return *this;
		}

		MdTask(MdTask&& rhs) noexcept
		{
			if (std::addressof(rhs) != this) {
				Swap_(rhs);
			}
		}

		MdTask& operator=(MdTask&) const = delete;
		MdTask(MdTask&) = delete;

		explicit MdTask(uint32_t count) noexcept
			:waitCount_(count)
		{}

	private:

		void Swap_(MdTask& rhs) noexcept
		{
			std::swap(continuation_, rhs.continuation_);
			std::swap(waitCount_, rhs.waitCount_);
		}
	};

	//Takes in some count of tasks and returns a single awaitable 
	// that will have a counter atached to all of the tasks given to it
	struct MultiDependentTask {
		struct FinalizeTask;
		struct promise_type;
		struct Awaiter;

		//Count of tasks this task is dependent on
		Counter_t waitCount_;
		//coroutine that should be continued once counter is complete
		std::coroutine_handle<> continuation_ = std::noop_coroutine();

		Awaiter operator co_await() & noexcept;

		~MultiDependentTask();

		MultiDependentTask& operator=(MultiDependentTask&&) noexcept;
		MultiDependentTask(MultiDependentTask&&) noexcept;

		MultiDependentTask& operator=(MultiDependentTask&) const = delete;
		MultiDependentTask(MultiDependentTask&) = delete;

		explicit MultiDependentTask(uint32_t count) noexcept;

	private:

		void Swap_(MultiDependentTask& rhs) noexcept;
	};

	struct MultiDependentTask::Awaiter {
		explicit Awaiter(MultiDependentTask& task) : task_(task)
		{}

		bool await_ready() const noexcept;

		bool await_suspend(std::coroutine_handle<> h) const noexcept;

		void await_resume() const noexcept;

		MultiDependentTask& task_;
	};

	struct MultiDependentTask::promise_type {
		auto get_return_object() noexcept {
			return std::coroutine_handle<MultiDependentTask::promise_type>::from_promise(*this);
		}

		std::suspend_always initial_suspend() const noexcept { return {}; }

		std::suspend_always final_suspend() const noexcept { return {}; }

		[[noreturn]]
		void unhandled_exception() const noexcept { std::terminate(); }

		void return_void() const {}

	};
}