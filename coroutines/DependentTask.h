#pragma once
#include <coroutine>
#include <atomic>
#include <iostream>

namespace Treadle
{
    struct DependentTask {

        struct promise_type {
            std::atomic_bool bCompleted = false;

            //function that is dependent on at least this function completing first
            // it may be waiting on other functions to complete as well
            std::coroutine_handle<> dependent;

            struct FinalizeTask {
                bool await_ready() const noexcept {
                    return false;
                }

                std::coroutine_handle<> await_suspend(std::coroutine_handle<DependentTask::promise_type> h) const noexcept {
                    std::cout << "finalize task suspend called \n";
                    //if this is a counter we want to check if we should resume the dependent call or else call the next job on the scheduler
                    h.promise().bCompleted = true;

                    //this is here for dependent tasks that never co-await. Either we should use a different type or do a better job of figuring out what the dependent is
                    if (h.promise().dependent) return h.promise().dependent;

                    //There are still tasks that need to be done before we can resume our dependent job. Continue
                    return std::noop_coroutine();
                }

                void await_resume() const noexcept {}
            };

            DependentTask get_return_object() noexcept {
                return DependentTask{ std::coroutine_handle<promise_type>::from_promise(*this) };
            }
            std::suspend_always initial_suspend() const {
                return {};
            }
            FinalizeTask final_suspend() const noexcept {
                return {};
            }

            void return_void() const noexcept {}

            [[noreturn]]
            void unhandled_exception() const {
                std::terminate();
            }
        };

        void Resume() const {
            coro_.resume();
        }
        bool Done() const { return coro_.done(); }

        struct Awaiter {
            explicit Awaiter(std::coroutine_handle<promise_type> currentCoro) : coro_(currentCoro)
            {}

            bool await_ready() const noexcept {
                return coro_.promise().bCompleted;
            }

            std::coroutine_handle<> await_suspend(std::coroutine_handle<> h) const {
                coro_.promise().dependent = h;
                return coro_;
            }

            void await_resume() const noexcept {
                //Do we need to do anything upon resuming?
            }

            std::coroutine_handle<promise_type> coro_;
        };

        Awaiter operator co_await() const && noexcept
        {
            return Awaiter{ coro_ };
        }

        void return_void() const {}

        DependentTask(DependentTask&& t) noexcept
            : coro_(t.coro_)
        {}

        DependentTask& operator=(DependentTask&& t) noexcept
        {
            if (std::addressof(t) != this) {
                DestroyThis_();
                coro_ = t.coro_;
                t.coro_ = nullptr;
            }
        };

        ~DependentTask() {
            DestroyThis_();
        }

        DependentTask& operator=(DependentTask& t) const = delete;
        DependentTask(DependentTask& t) = delete;

    private:
        explicit DependentTask(std::coroutine_handle<promise_type> h) noexcept
            : coro_(h)
        {   }

        inline void DestroyThis_() { /*This can be marked as const as coro_.detroy() is const, leaving as mutable better indicates its purpose*/
            if (coro_) coro_.destroy();
        }

        std::coroutine_handle<promise_type> coro_;
    };
}

