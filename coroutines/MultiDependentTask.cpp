#include "MultiDependentTask.h"
namespace Treadle
{
	MultiDependentTask::Awaiter MultiDependentTask::operator co_await() const && noexcept
	{
		return Awaiter{coro_};
	}

	void MultiDependentTask::Resume() const noexcept
	{
		coro_.resume();
	}

	bool MultiDependentTask::Done() const noexcept
	{
		return coro_.done();
	}

	MultiDependentTask::~MultiDependentTask()
	{
		if (coro_) coro_.destroy();
	}

	MultiDependentTask& MultiDependentTask::operator=(MultiDependentTask&& rhs)
	{
		if (std::addressof(rhs) != this) {
			Swap_(rhs);
		}
		return *this;
	}

	MultiDependentTask::MultiDependentTask(MultiDependentTask&& rhs)
	{
		Swap_(rhs);
	}

	void MultiDependentTask::Swap_(MultiDependentTask& rhs) noexcept
	{
		std::swap(coro_, rhs.coro_);
		rhs.counter_ = counter_.exchange(rhs.counter_);
	}


	bool MultiDependentTask::Awaiter::await_ready() const noexcept{
		return coro_.promise().waitCount_.load() == 0;
	}

	std::coroutine_handle<> MultiDependentTask::Awaiter::await_suspend(std::coroutine_handle<> h) const noexcept
	{
		coro_.promise().dependent_ = h;
		return coro_;
	}

	void MultiDependentTask::FinalizeTask::await_resume() const noexcept {}

	std::coroutine_handle<> MultiDependentTask::FinalizeTask::await_suspend(std::coroutine_handle<MultiDependentTask::promise_type> h) const noexcept
	{
		auto prevVal = h.promise().waitCount_.fetch_sub(1);
		//checking for 1 here as fetch_sub decrements and returns value prior to decrement.
		// saves us on doing 2 atomic operations. Final decrementer will resume dependent
		if (prevVal == 1) return h.promise().dependent_;

		return std::noop_coroutine();
	}

}

