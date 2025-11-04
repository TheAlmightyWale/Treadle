#include <coroutine>
#include <concepts>
#include "MultiTaskAwaitable.hpp"

namespace Treadle2
{
	template<typename TaskContainerType>
	class MultiTaskAwaitable;

	template<typename ReturnType>
	class MultiTask;

	//Task which wraps basic task and adds functionality needed for waiting on many tasks at once
	template<typename ReturnType>
	class MultiTaskPromise final{
	public:

		using CoroutineHandle = std::coroutine_handle<MultiTaskPromise<ReturnType>>;

		MultiTaskPromise() noexcept
		{}

		auto GetReturnObject() noexcept
		{
			return CoroutineHandle::from_promise(*this);
		}

		//We should assure we always start paused so we can correctly set the counter before starting
		std::suspend_always initial_suspend() noexcept
		{
			return {};
		}

		auto final_suspend() noexcept
		{
			struct Awaiter
			{
				bool await_ready() const noexcept {
					return false;
				}
				void await_suspend(std::coroutine_handle<MultiTaskPromise> coro) noexcept
				{
					coro.promise().counter_.HandleDependentTaskCompletion();
				}
				void await_resume() noexcept {}
			};

			return Awaiter{};
		}

		auto YieldValue(ReturnType&& value) noexcept
		{
			result_ = std::addressof(value);
			return final_suspend();
		}

		void Start(MultiCounter* counter) noexcept
		{
			counter_ = counter;
			CoroutineHandle::from_promise(*this).resume();
		}

		ReturnType& Value() & noexcept
		{
			return *result_;
		}

		ReturnType&& Value() && noexcept
		{
			return std::forward<ReturnType>(*result_);
		}

	private:
		MultiCounter* counter_ = nullptr;
		std::add_pointer_t<ResultType> result_ = nullptr;
	};

	template<>
	class MultiTaskPromise<void> final {
		using CoroutineHandle = std::coroutine_handle<MultiTaskPromise<void>>;

		MultiTaskPromise() noexcept
		{
		}

		auto GetReturnObject() noexcept
		{
			return CoroutineHandle::from_promise(*this);
		}

		std::suspend_always initial_suspend() noexcept
		{
			return {};
		}

		auto final_suspend() noexcept
		{
			struct Awaiter
			{
				bool await_ready() const noexcept {
					return false;
				}
				void await_suspend(std::coroutine_handle<MultiTaskPromise> coro) noexcept
				{
					coro.promise().counter_->HandleDependentTaskCompletion();
				}
				void await_resume() noexcept {}
			};

			return Awaiter{};
		}

		auto YieldValue() noexcept
		{
			return final_suspend();
		}

		void Start(MultiCounter* counter) noexcept
		{
			counter_ = counter;
			CoroutineHandle::from_promise(*this).resume();
		}

	private:
		MultiCounter* counter_ = nullptr;
	};

	template<typename ReturnType>
	class MultiTask final {
		using promise_type = MultiTaskPromise<ReturnType>;
		using CoroutineHandle = promise_type::CoroutineHandle;

		MultiTask(CoroutineHandle handle) noexcept
			: coro_(handle)
		{}

		MultiTask(MultiTask&& other) noexcept
			: coro_(std::exchange(other.coro_, std::noop_coroutine())
		{}

		~MultiTask() noexcept
		{
			if (coro_) coro_.destroy();
		}

		MultiTask(MultiTask const) = delete;
		MultiTask& operator=(MultiTask const) = delete;

		decltype(auto) Value() & noexcept
		{
			return coro_.promise().Value();
		}

		decltype(auto) Value() && noexcept
		{
			return std::move(coro_.promise()).Value();
		}

	private:
		void Start(MultiCounter& counter) noexcept
		{
			coro_.promise().Start(counter);
		}

		CoroutineHandle coro_ = std::noop_coroutine();
	};

	template<typename AwaitableType, std::movable ReturnType >
	MultiTask<ReturnType> MakeMuliTask(AwaitableType awaitable)
	{
		co_yield co_await awaitable;
	}


	template<typename AwaitableType, std::is_void ReturnType>
	MultiTask<void> MakeMuliTask(AwaitableType awaitable)
	{
		co_await awaitable;
	}

	//TODO add std::reference_wrapper versions

}