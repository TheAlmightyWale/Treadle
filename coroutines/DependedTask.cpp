#include "DependedTask.h"
#include <coroutine>
#include <utility>

#include <iostream>

namespace Treadle {
	DependedTask::Awaiter DependedTask::operator co_await() const&& noexcept
	{
		return Awaiter{ coro_ };
	}

	void DependedTask::Resume() const noexcept
	{
		coro_.resume();
	}

	bool DependedTask::Done() const noexcept
	{
		return coro_.done();
	}

	DependedTask& DependedTask::operator=(DependedTask&& rhs) noexcept
	{
		if (std::addressof(rhs) != this) {
			Swap_(rhs);
		}
		return *this;
	}

	DependedTask::DependedTask(DependedTask&& t) noexcept
		: coro_(std::exchange(t.coro_, {}))
	{}

	DependedTask::DependedTask(std::coroutine_handle<promise_type> h) noexcept
		:coro_(h)
	{}

	DependedTask::~DependedTask()
	{
		if (coro_) coro_.destroy();
	}

	void DependedTask::Swap_(DependedTask& rhs) noexcept
	{
		std::swap(coro_, rhs.coro_);
	}

	DependedTask::Awaiter::Awaiter(std::coroutine_handle<promise_type> coro)
		: coro_(coro)
	{}

	bool DependedTask::Awaiter::await_ready() const noexcept {
		return false;
	}

	std::coroutine_handle<> DependedTask::Awaiter::await_suspend(std::coroutine_handle<> h) const noexcept
	{
		return coro_;
	}

	void DependedTask::Awaiter::await_resume() const noexcept {}

	DependedTask DependedTask::promise_type::get_return_object() noexcept
	{
		return DependedTask{ std::coroutine_handle<promise_type>::from_promise(*this) };
	}

	//lazy started
	std::suspend_always DependedTask::promise_type::initial_suspend() const noexcept
	{
		return {};
	}

	DependedTask::FinalizeTask DependedTask::promise_type::final_suspend() const noexcept 
	{
		return {};
	}

	void DependedTask::promise_type::return_void() const noexcept
	{}

	bool DependedTask::FinalizeTask::await_ready() const noexcept
	{
		return false;
	}

	void DependedTask::FinalizeTask::await_resume() const noexcept
	{}

	std::coroutine_handle<> DependedTask::FinalizeTask::await_suspend(std::coroutine_handle<DependedTask::promise_type> h) const noexcept
	{
		(*h.promise().pDependedTasksRemaining)--;
		std::cout << "depended= " << *h.promise().pDependedTasksRemaining << "\n";
		if (*h.promise().pDependedTasksRemaining <= 0) return *(h.promise().dependentCoro);

		return std::noop_coroutine();
	}
}

