#include "gtest/gtest.h"
#include "TestUtils.hpp"

#include <Wait.hpp>

namespace Treadle2
{
	// create coroutine type which will await a certain number of times before continuing
	struct Countdown
	{
		using promise_type = TestPromise;

		bool isReady() const noexcept { return count_ == 0; }

		auto operator co_await() noexcept
		{
			class awaiter
			{
			public:
				awaiter(Countdown &cd) noexcept
					: countdown_(cd)
				{
				}

				bool await_ready() const noexcept
				{
					return countdown_.isReady();
				}

				bool await_suspend(std::coroutine_handle<> coro)
				{
					countdown_.continuation_ = coro;
					countdown_.count_--;

					return true;
				}

				void await_resume() noexcept {}

			private:
				Countdown &countdown_;
			};

			return awaiter{*this};
		}

	private:
		uint32_t count_ = 3;
		std::coroutine_handle<> continuation_ = std::noop_coroutine();
	};

	Task<void> WaitOnCountdown(Countdown &cd) noexcept
	{
		while (!cd.isReady())
		{
			co_await cd;
		}
	}

	TEST(SyncWaitTests, WaitOne)
	{
		Countdown countdown;
		Task<void> task = WaitOnCountdown(countdown);

		EXPECT_FALSE(task.Done());
		SyncWait(task);

		EXPECT_TRUE(task.Done());
	}

	TEST(SyncWaitTests, WaitMany)
	{
		Countdown countdown;
		Task<void> task1 = WaitOnCountdown(countdown);
		Task<void> task2 = WaitOnCountdown(countdown);

		EXPECT_FALSE(task1.Done());
		EXPECT_FALSE(task2.Done());
		SyncWait(task1, task2);

		EXPECT_TRUE(task1.Done());
		EXPECT_TRUE(task2.Done());
	}

	TEST(AsyncWaitTests, AsyncWaitOne)
	{
	}

	TEST(AsyncWaitTests, AsyncWaitMany)
	{
	}
}