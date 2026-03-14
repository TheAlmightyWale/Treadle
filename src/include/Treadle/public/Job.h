#pragma once
#include <utility>
#include <coroutine>
#include "TaskTraits.hpp"

namespace Treadle
{
    using Job = std::coroutine_handle<>;
}	