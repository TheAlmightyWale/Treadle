#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>

namespace Treadle
{
    // Very basic MpmcQueue, we're not going to care about performance right now
    template <typename T>
    class MpmcQueue
    {
    public:
        void push(const T &value)
        {
            {
                std::lock_guard<std::mutex> lock(mutex_);
                queue_.push(value);
            }
        }

        std::optional<T> try_pop()
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (queue_.empty())
            {
                return std::nullopt;
            }
            std::optional<T> ret = std::move(queue_.front());
            queue_.pop();
            return ret;
        }

        bool empty() const noexcept
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return queue_.empty();
        }

    private:
        std::queue<T> queue_;
        mutable std::mutex mutex_;
    };
}