#pragma once
#include <atomic>

namespace Treadle
{
	using CounterType = uint32_t;
	using Counter_t = std::atomic<uint32_t>;
}