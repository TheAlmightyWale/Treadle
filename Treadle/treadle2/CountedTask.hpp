//TODO: we must be able to compress this and Task into a single class with mix-in policies or the like
// rather than have so much duplication
#pragma once
#include "TaskTraits.hpp"

// Task that is created as part of waiting for many tasks, will decrement counter and
//Will then execute depending task automatically when all tasks are done
namespace Treadle2::Detail
{
	using CounterType = uint32_t;
	using Counter_t = std::atomic<CounterType>;

	template<typename ReturnType>
	struct CountedTask;

	struct CountedPromiseBase
	{
		struct DecrementFinalAwaitable
		{
		public:
			bool await_ready() const noexcept {
				return false;
			}

			template<typename PromiseType>
			std::coroutine_handle<> await_suspend(std::coroutine_handle<CountedPromiseBase> h)  noexcept {
				h.promise().dependedTasksCount_--;
				if (h.promise().dependedTasksCount_ == 0) {
					return h.promise().continuation_;
				}
				return std::noop_coroutine();
			}

			void await_resume() const noexcept {}

		private:
		};

		explicit CountedPromise(Counter_t& count)
			: dependedTasksCount_(count)
		{}

		std::suspend_always initial_suspend() const noexcept {
			return {};
		}

		DecrementFinalAwaitable final_suspend() const noexcept {
			return {};
		}

		void set_continuation(std::coroutine_handle<> coro)
		{
			continuation_ = coro;
		}

	private:
		std::coroutine_handle<> continuation_ = std::noop_coroutine();
		//TODO who owns counter memory? - it should be the overall task that waits on the counted tasks
		Counter_t& dependedTasksCount_; // Number of tasks that must complete before this task can continue
	};

	template<typename ReturnType>
	struct CountedPromise : CountedPromiseBase
	{
		using TaskType = CountedTask<ReturnType>;

		TaskType get_return_object() noexcept;

		[[nodiscard]]
		ReturnType&& result() {
			return std::move(result_);
		}

		void unhandled_exception() const noexcept {
			std::terminate();
		}

		void return_value(ReturnType&& value) noexcept {
			result_ = value;
		}

		void return_value(ReturnType& value) {
			result_ = value;
		}
	private:
		ReturnType result_;
	};

	template<>
	struct CountedPromise<void> : CountedPromiseBase{
		Task<void> get_return_object() noexcept;

		void unhandled_exception() const noexcept {
			std::terminate();
		}

		void return_void() const noexcept {}

		void result() {}

	private:
	}

	template<typename ReturnType>
	struct CountedTask
	{
	public:
		using promise_type = CountedPromise<ReturnType>;

		CountedTask(CountedTask const&) = delete;
		CountedTask& operator=(CountedTask const&) = delete;

		CountedTask(CountedTask&& t) noexcept
			: coro_(std::move(t.coro_))
		{}

		CountedTask& operator=(CountedTask&& t) noexcept {
			Swap(t);
			return *this;
		}

		~CountedTask() {
			if (coro_) coro_.destroy();
		}

		void Resume() { coro_.resume(); }
		bool Done() { return coro_.done(); }
		ReturnType Value() {
			return coro_.promise().result();
		}

		void Swap(CountedTask& other) noexcept {
			std::swap(coro_, other.coro_);
		}

		struct AwaitableBase
		{
			AwaitableBase(std::coroutine_handle<promise_type> coro)
				: coro_(coro)
			{}

			bool await_ready() const noexcept {
				return false;
			}

			std::coroutine_handle<> await_suspend(std::coroutine_handle<> h)
			{
				coro_.promise().set_continuation(h);
				return coro_;
			}

		protected:
			std::coroutine_handle<promise_type> coro_;
		};

		//R-Value Task
		auto operator co_await() const&& noexcept
		{
			struct Awaiter : AwaitableBase
			{
				decltype(auto) await_resume() const noexcept
				{
					return coro_.promise().result();
				}
			};

			return Awaiter{ coro_ };
		}


		//L-value Task
		auto operator co_await() const& noexcept
		{
			struct Awaiter : AwaitableBase
			{
				decltype(auto) await_resume() const noexcept
				{
					return std::move(coro_.promise()).result();
				}
			};

			return Awaiter{ coro_ };
		}

	private:
		std::coroutine_handle<promise_type> coro_;
	};

	template<typename ReturnType>
	Task<ReturnType> Promise<ReturnType>::get_return_object() noexcept {
		return Task{ std::coroutine_handle<Promise>::from_promise(*this) };
	}

	inline Task<void> Promise<void>::get_return_object() noexcept {
		return Task{ std::coroutine_handle<Promise>::from_promise(*this) };
	}
}

