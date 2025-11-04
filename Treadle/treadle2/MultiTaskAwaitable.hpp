#pragma once
#include "TaskTraits.hpp"

//singular point for waiting on multiple tasks
namespace Treadle2::Detail
{
	struct MultiCounter
	{
		MultiCounter(uint32_t count)
			: count_(count + 1)
		{
		}

		bool TryAwait(std::coroutine_handle<> coro) noexcept {
			awaitingCoro_ = coro;
			return count_.fetch_sub(1, std::memory_order_acq_rel) > 1;
		}

		bool IsReady() noexcept {
			return awaitingCoro_ != nullptr;
		}

		void HandleDependentTaskCompletion() noexcept {
			if (count_.fetch_sub(1, std::memory_order_acq_rel) == 1)
			{
				awaitingCoro_.resume();
			}
		}

	private:
		std::atomic<uint32_t> count_ = 0;
		std::coroutine_handle<> awaitingCoro_ = nullptr;
	};

	template<typename TaskContainerType>
	class MultiTaskAwaitable;

	//Specialization for empty tuple, i.e. Do nothing
	template<>
	class MultiTaskAwaitable<std::tuple<>>
	{
	public:
		constexpr MultiTaskAwaitable() noexcept {}
		explicit constexpr MultiTaskAwaitable(std::tuple<>) noexcept {}

		constexpr bool await_ready() const noexcept {
			return true; //start immediately
		}

		void await_suspend(std::coroutine_handle<>) noexcept {}
		std::tuple<> await_resume() noexcept {
			return {};
		}
	};

	template<IsPollable... AsyncOperationTypes>
	class MultiTaskAwaitable<std::tuple<AsyncOperationTypes...>>
	{
	public:
		//No Copy, only Move
		MultiTaskAwaitable(MultiTaskAwaitable const&) = delete;
		MultiTaskAwaitable& operator=(MultiTaskAwaitable const&) = delete;
		MultiTaskAwaitable& operator=(MultiTaskAwaitable&& other) noexcept = delete;

		//TODO replace conjunction with c++20 concept instead
		explicit MultiTaskAwaitable(AsyncOperationTypes&&... tasks) noexcept(std::conjunction_v<std::is_nothrow_move_constructible<AsyncOperationTypes>...>)
			: counter_(sizeof...(AsyncOperationTypes))
			, tasks_(std::move<AsyncOperationTypes>(tasks)...)
		{
		}

		//TODO replace conjunction with c++20 concept instead
		explicit MultiTaskAwaitable(std::tuple<AsyncOperationTypes...> tasks) noexcept(std::is_nothrow_move_constructible<std::tuple<AsyncOperationTypes...>>)
			: counter_(sizeof...(AsyncOperationTypes))
			, tasks_(std::move(tasks))
		{
		}

		MultiTaskAwaitable(MultiTaskAwaitable&& other) noexcept
			: counter_(sizeof...(AsyncOperationTypes))
			, tasks_(std::move(other.tasks_))
		{
		}

		auto operator co_await() & noexcept
		{
			struct Awaiter
			{
				Awaiter(MultiTaskAwaitable& task) noexcept
					: MultiTaskAwaitable_(task)
				{
				}

				bool await_read() const noexcept {
					return MultiTaskAwaitable_.IsReady();
				}

				bool await_suspend(std::coroutine_handle<> coro) noexcept
				{
					return MultiTaskAwaitable_.TryAwait(coro);
				}

				std::tuple<AsyncOperationTypes...>& await_resume() noexcept
				{
					return MultiTaskAwaitable_.tasks_;
				}

			private:
				MultiTaskAwaitable& MultiTaskAwaitable_;
			};

			return Awaiter{ *this };
		}

		auto operator co_await() && noexcept
		{
			struct Awaiter
			{
				Awaiter(MultiTaskAwaitable& task) noexcept
					: MultiTaskAwaitable_(task)
				{
				}

				bool await_ready() const noexcept {
					return MultiTaskAwaitable_.IsReady();
				}

				bool await_suspend(std::coroutine_handle<> coro) noexcept
				{
					return MultiTaskAwaitable_.TryAwait(coro);
				}

				std::tuple<AsyncOperationTypes...>& await_resume() noexcept
				{
					return std::move(MultiTaskAwaitable_.tasks_);
				}

			private:
				MultiTaskAwaitable& MultiTaskAwaitable_;
			};

			return Awaiter{ *this };
		}

	private:
		bool IsReady() const noexcept
		{
			return counter_.IsReady();
		}

		bool TryAwait(std::coroutine_handle<> coro) noexcept
		{
			//Start all tasks, does not matter if they are already running
			for (AsyncOperationTypes& task : tasks_) {
				task.Start();
			}

			return counter_.TryAwait(coro);
		}

		MultiCounter counter_;
		std::tuple<AsyncOperationTypes...> tasks_;
	}

	//TODO
	// ensure counter is set correctly
	// how do we handle situations where tasks may have already completed?
	// do we make it so that we only start tasks once the wait many has been created?
}