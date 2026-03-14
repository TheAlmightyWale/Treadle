#pragma once
#include <concepts>
#include <coroutine>
#include "AtomicCounter.h"

namespace Treadle
{

template <typename T>
concept IsPollable = requires(T obj) {
	{ obj.Done() } -> std::same_as<bool>;
};

template <typename T>
concept IsTask = requires(T obj, Counter_t* counter, std::coroutine_handle<> handle) {
	{ obj.InitializeCounter(uint32_t{0}) } -> std::same_as<void>;
	{ obj.SetCounter(counter) } -> std::same_as<void>;
	{ obj.SetContinuation(handle) } -> std::same_as<void>;
	{ obj.GetCounter() } -> std::same_as<Counter_t&>;
	{ obj.GetCoroutine() } -> std::same_as<std::coroutine_handle<>>;
};

}