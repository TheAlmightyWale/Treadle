#include "MultiDependentTask.h"

#include <iostream>

namespace Treadle
{
	MultiDependentTask::Awaiter MultiDependentTask::operator co_await() & noexcept
	{
		return Awaiter{ *this };
	}

	MultiDependentTask::MultiDependentTask(uint32_t count) noexcept
		: waitCount_(count)
	{
		std::cout << "Constructing multi\n";
	}

	MultiDependentTask::~MultiDependentTask()
	{
		std::cout << "Destroying Multi \n";
		//if (coro_) coro_.destroy();
	}

	MultiDependentTask& MultiDependentTask::operator=(MultiDependentTask&& rhs) noexcept
	{
		if (std::addressof(rhs) != this) {
			Swap_(rhs);
		}
		return *this;
	}

	MultiDependentTask::MultiDependentTask(MultiDependentTask&& rhs) noexcept
	{
		Swap_(rhs);
	}

	void MultiDependentTask::Swap_(MultiDependentTask& rhs) noexcept
	{
	}

	bool MultiDependentTask::Awaiter::await_ready() const noexcept{
		//return task_.waitCount_.load() == 0;
		return false;
	}

	void MultiDependentTask::Awaiter::await_resume() const noexcept {}

	bool MultiDependentTask::Awaiter::await_suspend(std::coroutine_handle<> h) const noexcept
	{
		task_.continuation_= h;
		return task_.waitCount_.load() > 0? true : false;
	}

	//void MultiDependentTask::FinalizeTask::await_resume() const noexcept {}

	//std::coroutine_handle<> MultiDependentTask::FinalizeTask::await_suspend(std::coroutine_handle<MultiDependentTask::promise_type> h) const noexcept
	//{
	//	auto prevVal = h.promise().waitCount_.fetch_sub(1);
	//	//checking for 1 here as fetch_sub decrements and returns value prior to decrement.
	//	// saves us on doing 2 atomic operations. Final decrementer will resume dependent
	//	if (prevVal == 1) return h.promise().dependent_;

	//	return std::noop_coroutine();
	//}

}

