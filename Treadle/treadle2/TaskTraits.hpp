#pragma once
#include <concepts>

template <typename T>
concept IsPollable = requires(T obj) {
	{ obj.Done() } -> std::same_as<bool>;
};