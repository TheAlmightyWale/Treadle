#pragma once
#include <concepts>
#include <coroutine>

template <typename T>
concept IsPollable = requires(T obj) {
	{ obj.Done() } -> std::same_as<bool>;
};