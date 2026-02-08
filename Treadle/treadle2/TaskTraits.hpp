#pragma once
#include <concepts>

template <typename T>
concept IsPollable = requires(T obj) {
	{ obj.Done() } -> std::same_as<bool>;
};

template <typename T>
concept IsTask = requires(T obj) {
	{ obj.SetContinuation() } -> std::same_as<void>;
	{ obj.SetCounter() } -> std::same_as<void>;
};