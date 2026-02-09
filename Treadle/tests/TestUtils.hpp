#pragma once

#include <coroutine>
#include "Task.hpp"

namespace TreadleTest
{
	using namespace Treadle;

	struct TestPromise
	{
		std::suspend_always initial_suspend() const noexcept
		{
			return {};
		}

		std::suspend_always final_suspend() const noexcept
		{
			return {};
		}
	};

	struct TestEvent
	{
		using promise_type = TestPromise;

		bool isSet() const noexcept { return bSet; }

		void set() noexcept { bSet = true; }

		auto operator co_await() noexcept
		{
			class awaiter
			{
			public:
				awaiter(TestEvent &event) noexcept
					: event_(event)
				{
				}

				bool await_ready() const noexcept
				{
					return event_.isSet();
				}

				bool await_suspend(std::coroutine_handle<> coro)
				{
					event_.continuation_ = coro;

					return true;
				}

				void await_resume() noexcept {}

			private:
				TestEvent &event_;
			};

			return awaiter{*this};
		}

	private:
		bool bSet = false;
		std::coroutine_handle<> continuation_ = std::noop_coroutine();
	};

	struct FlagSetter
	{
		FlagSetter() = default;
		FlagSetter(FlagSetter &) = default;
		FlagSetter(FlagSetter &&other)
			: bDependencyCompleteFlag(other.bDependencyCompleteFlag)
		{
		}

		bool GetFlag() const noexcept { return bDependencyCompleteFlag; }

		Task<void> WaitAndSet(Task<void> &wait)
		{
			co_await wait;

			bDependencyCompleteFlag = true;
		}

	private:
		bool bDependencyCompleteFlag = false;
	};
}