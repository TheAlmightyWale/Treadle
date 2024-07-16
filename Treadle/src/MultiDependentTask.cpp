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
		std::swap(continuation_, rhs.continuation_);
	}

	bool MultiDependentTask::Awaiter::await_ready() const noexcept {
		return false;
	}

	void MultiDependentTask::Awaiter::await_resume() const noexcept {}

	bool MultiDependentTask::Awaiter::await_suspend(std::coroutine_handle<> h) const noexcept
	{
		task_.continuation_ = h;
		return task_.waitCount_.load() > 0 ? true : false;
	}

}