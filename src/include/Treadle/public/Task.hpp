#pragma once
#include <coroutine>
#include <atomic>
#include <exception>
#include <utility>
#include "TaskTraits.hpp"
#include "AtomicCounter.h"
#include "IScheduler.h"
#include "job.h"

//temp
#include <iostream>

namespace Treadle
{
	namespace Detail
	{
		static uint32_t tid = 0;
	}

	template <typename ReturnType>
	struct Task;

	struct PromiseBase
	{
		struct ResumeAwaitable
		{
		public:
			ResumeAwaitable(std::coroutine_handle<> continuation)
				: continuation_(continuation)
			{}

			// always suspend
			bool await_ready() const noexcept
			{
				return false;
			}

			template <typename PromiseType>
			std::coroutine_handle<> await_suspend(std::coroutine_handle<PromiseType>) noexcept
			{
				return continuation_;
			}

			void await_resume() const noexcept {}

		private:
			std::coroutine_handle<> continuation_;
		};

		std::suspend_always initial_suspend() const noexcept
		{
			return {};
		}

		ResumeAwaitable final_suspend() const noexcept
		{
			if (dependantCounter && dependantCounter->fetch_sub(1, std::memory_order_acq_rel) == 1)
			{
				return {continuation_};
			}

			return ResumeAwaitable(std::noop_coroutine());
		}

		void set_continuation(std::coroutine_handle<> coro)
		{
			continuation_ = coro;
		}

		void set_counter(Counter_t *counter)
		{
			dependantCounter = counter;
		}

		void init_dependee_counter(uint32_t dependants){
			dependeeCounter = dependants;
		}

		Counter_t& get_counter()
		{
			return dependeeCounter;
		}

	private:
		//Counter to decrement when this task completes, potentially running it's continuation if it hits 0
		Counter_t* dependantCounter = nullptr;
		std::coroutine_handle<> continuation_ = std::noop_coroutine();
		//Counter that tasks this task depends on can decrement to know when to continue them
		Counter_t dependeeCounter = 0;
	};

	// TODO Policy type these? We only want custom behavior for
	//  final suspend classes
	// Along with this we will want to also hide implementation classes

	template <typename ReturnType>
	struct Promise : PromiseBase
	{
		Task<ReturnType> get_return_object() noexcept;

		void unhandled_exception() const noexcept
		{
			std::terminate();
		}

		[[nodiscard]]
		ReturnType &&result()
		{
			return std::move(result_);
		}

		void return_value(ReturnType &&value)
		{
			result_ = value;
		}

		void return_value(ReturnType &value)
		{
			result_ = value;
		}

	private:
		ReturnType result_;
	};

	template <>
	struct Promise<void> : PromiseBase
	{

		Task<void> get_return_object() noexcept;

		void unhandled_exception() const noexcept
		{
			std::terminate();
		}

		void return_void() const noexcept {}

		void result() {}

	private:
	};

	template <typename ReturnType>
	struct Task
	{
	public:
		using promise_type = Promise<ReturnType>;

		Task(Task const &) = delete;
		Task &operator=(Task const &) = delete;

		Task(Task &&t) noexcept
		{
			Swap(t);
		}

		Task &operator=(Task &&t) noexcept
		{
			Swap(t);
			return *this;
		}

		explicit Task(std::coroutine_handle<promise_type> coro) noexcept
			: coro_(coro)
		{}

		~Task()
		{
			if (coro_) {
				coro_.destroy();
			}
				
		}

		void Resume() { coro_.resume(); }
		bool Done() { return coro_.done(); }

		void InitializeCounter(uint32_t dependantCount)
		{
			coro_.promise().init_dependee_counter(dependantCount);
		}

		void SetCounter(Counter_t* counter)
		{
			coro_.promise().set_counter(counter);
		}

		void SetContinuation(std::coroutine_handle<> continuation)
		{
			coro_.promise().set_continuation(continuation);
		}

		Counter_t& GetCounter()
		{
			return coro_.promise().get_counter();
		}

		std::coroutine_handle<> GetCoroutine() const
		{
			return coro_;
		}

		ReturnType Value()
		{
			return coro_.promise().result();
		}

		void Swap(Task &other) noexcept
		{
			std::swap(coro_, other.coro_);
		}

		struct AwaitableBase
		{
			AwaitableBase(std::coroutine_handle<promise_type> coro)
				: coro_(coro)
			{}

			bool await_ready() const noexcept
			{
				return false;
			}

			void await_suspend(std::coroutine_handle<promise_type> h)
			{
				//Set up new dependency we have awaited
				Counter_t& dependantCounter = h.promise().get_counter();
				dependantCounter += 1;
				coro_.promise().set_continuation(h);
				coro_.promise().set_counter(&dependantCounter);
				IScheduler::Get().Schedule(coro_);
			}

		protected:
			std::coroutine_handle<promise_type> coro_;
		};

		// R-Value Task
		auto operator co_await() const && noexcept
		{
			struct Awaiter : AwaitableBase
			{
				decltype(auto) await_resume() const noexcept
				{
					return coro_.promise().result();
				}
			};

			return Awaiter{coro_};
		}

		// L-value Task
		auto operator co_await() const & noexcept
		{
			struct Awaiter : AwaitableBase
			{
				decltype(auto) await_resume() const noexcept
				{
					return std::move(this->coro_.promise()).result();
				}
			};

			return Awaiter{coro_};
		}

		uint32_t const m_id = Detail::tid++;

	private:
		std::coroutine_handle<promise_type> coro_;
	};

	template <typename ReturnType>
	Task<ReturnType> Promise<ReturnType>::get_return_object() noexcept
	{
		return Task{std::coroutine_handle<Promise>::from_promise(*this)};
	}

	inline Task<void> Promise<void>::get_return_object() noexcept
	{
		return Task{std::coroutine_handle<Promise>::from_promise(*this)};
	}

}