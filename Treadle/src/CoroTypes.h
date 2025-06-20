#pragma once
#include <concepts>

#include "Task.h"



namespace Treadle
{
	//Does nothing
	struct BasicPromise {
		Task<BasicPromise> get_return_object() noexcept {
			return Task<BasicPromise>{ std::coroutine_handle<BasicPromise>::from_promise(*this) };
		}

		std::suspend_always initial_suspend() const noexcept {
			return {};
		}

		std::suspend_always final_suspend() const noexcept {
			return {};
		}

		void unhandled_exception() const noexcept {}

		void return_void() const noexcept {}
	};

	struct DependedPromise {


		struct DecrementOnFinalSuspend {

		};
	};

	//Lazy initialized awaiter
	template<typename Promise_t>
	struct LazyAwaiter {
		explicit LazyAwaiter(std::coroutine_handle<Promise_t> coro)
			: coro_(coro)
		{}

		bool await_ready() const noexcept { return false; }
		std::coroutine_handle<> await_suspend(std::coroutine_handle<> h) const noexcept
		{
			return coro_;
		}

		void await_resume() const noexcept
		{}

		std::coroutine_handle<Promise_t> coro_;
	};

}

