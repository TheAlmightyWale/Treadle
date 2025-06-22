#pragma once
#include <coroutine>
#include <atomic>

namespace Treadle2
{
	template<typename ReturnType>
	struct Task;

	using CounterType = uint32_t;
	using Counter_t = std::atomic<CounterType>;

	struct PromiseBase
	{
		struct FinalAwaitable
		{
		public:
			bool await_ready() const noexcept {
				return false;
			}

			template<typename PromiseType>
			std::coroutine_handle<> await_suspend(std::coroutine_handle<PromiseType> h)  noexcept{
				return h.promise().continuation_;
			}

			void await_resume() const noexcept {}

		private:
		};

		PromiseBase() = default;

		std::suspend_always initial_suspend() const noexcept {
			return {};
		}

		FinalAwaitable final_suspend() const noexcept {
			return {};
		}

		void set_continuation(std::coroutine_handle<> coro)
		{
			continuation_ = coro;
		}

	private:
		std::coroutine_handle<> continuation_ = std::noop_coroutine();
	};

	//TODO Policy type these? We only want custom behavior for
	// final suspend classes
	//Along with this we will want to also hide implementation classes

	template<typename ReturnType>
	struct Promise : PromiseBase {

		Task<ReturnType> get_return_object() noexcept;

		void unhandled_exception() const noexcept {
			std::terminate();
		}

		[[nodiscard]]
		ReturnType result() {
			return result_;
		}

		void return_value(ReturnType&& value) {
			result_ = value;
		}

		void return_value(ReturnType& value) {
			result_ = value;
		}

	private:
		Counter_t remainingTasks_;
		ReturnType result_;
	};

	template<>
	struct Promise<void> : PromiseBase {

		Task<void> get_return_object() noexcept;

		void unhandled_exception() const noexcept {
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

		Task(Task&& t) noexcept
			: coro_(std::exchange(t.coro_, {}))
		{
		}

		explicit Task(std::coroutine_handle<promise_type> coro) noexcept
			: coro_(coro)
		{
		}

		~Task() {
			if (coro_) coro_.destroy();
		}

		void Resume() { coro_.resume(); }
		bool Done() { return coro_.done(); }
		ReturnType Value() {
			return coro_.promise().result();
		}

		struct AwaitableBase
		{
			AwaitableBase(std::coroutine_handle<promise_type> coro)
				: coro_(coro)
			{
			}

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
		auto operator co_await() const && noexcept
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
					return std::move(coro_.promise().result());
				}
			};

			return Awaiter{coro_};
		}

	private:
		std::coroutine_handle<promise_type> coro_;

	};

	template<typename ReturnType>
	Task<ReturnType> Promise<ReturnType>::get_return_object() noexcept {
		return Task{ std::coroutine_handle<Promise>::from_promise(*this) };
	}

	Task<void> Promise<void>::get_return_object() noexcept {
		return Task{ std::coroutine_handle<Promise>::from_promise(*this) };
	}

}