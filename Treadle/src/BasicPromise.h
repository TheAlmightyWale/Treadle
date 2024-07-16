#pragma once
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
}

