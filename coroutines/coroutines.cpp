#include <iostream>
#include <coroutine>
#include <queue>
#include "DependentTask.h"

static int gid = 0;
static int getGid() {
    int num = gid;
    ++gid;
    return num;
}

static int agid = 0;
static int getAGid() {
    int num = agid;
    ++agid;
    return num;
}

static int tgid = 0;
static int getTGid() {
    int num = tgid;
    ++tgid;
    return num;
}

struct Task {
    struct promise_type {
        Task get_return_object() noexcept {
            std::cout << "Getting return object pt:" << m_id << "\n";
            return Task{ std::coroutine_handle<promise_type>::from_promise(*this) };
        }
        std::suspend_always initial_suspend() const {
            std::cout << "Initial suspend pt:" << m_id << "\n";
            return {};
        }
        std::suspend_always final_suspend() const noexcept {
            std::cout << "Final suspend pt:" << m_id << "\n";
            return {};
        }

        void unhandled_exception() const {
            std::cout << "exception got pt:" << m_id << "\n";
            std::terminate();
        }

        void return_void() const noexcept {
            std::cout << "returning void pt:" << m_id << "\n";
        }

        //std::coroutine_handle<> continuation;
        int m_id = getGid();
    };

    Task(Task&& t) noexcept
        : coro_(std::exchange(t.coro_, {}))
    {
        std::cout << "Creating Task: " << m_id <<"\n";
    }

    ~Task() {
        if (coro_) coro_.destroy();
        std::cout << "destroying Task: " << m_id << "\n";
    }

    void Resume() { coro_.resume(); }
    bool Done() { return coro_.done(); }

    struct Awaiter {

        explicit Awaiter(std::coroutine_handle<promise_type> coro_) : coro_(coro_) {
            std::cout << "constructing aw: " << m_id << "\n";
            std::cout << "coro promise: " << coro_.promise().m_id << "\n";
        }

        bool await_ready() const noexcept {
            std::cout << "await_ready aw:" << m_id << "\n";
            return false;
        }

        std::coroutine_handle<> await_suspend(std::coroutine_handle<promise_type> h) {
            std::cout << "await_suspend aw:" << m_id << "\n";
            std::cout << "got coro promise: " << h.promise().m_id << " set as cont \n";
            return coro_;
        }

        void await_resume() const noexcept {
            std::cout << "await_resume aw:" << m_id << "\n";
        }

        std::coroutine_handle<promise_type> coro_;
        int m_id = getAGid();
    };

    Awaiter operator co_await() && noexcept
    {
        auto aw = Awaiter{ coro_ };
        std::cout << "Co_awaitng returning aw: " << aw.m_id << " from task: " << m_id << "\n";
        return aw;
    }

private:
    explicit Task(std::coroutine_handle<promise_type> h) noexcept
        : coro_(h)
    {
        std::cout << "Creating Task: " << m_id << " with coro prom: " << h.promise().m_id << "\n";
    }
    std::coroutine_handle<promise_type> coro_;
    int m_id = getTGid();
};

Task WayDown() {
    std::cout << "down\n";
    co_return;
}

Task Synchro() {
    std::cout << "synchro\n";
    co_await WayDown();
}

Task LoopSync(int count) {
    for (int i = 0; i < count; ++i)
    {
        std::cout << "~~~~ doing loop: " << i << " ~~~~~~~~\n";
        co_await Synchro();
        std::cout << "~~~~~ loop fin ~~~~~~~\n";
    }
}


Treadle::DependentTask doFlip() {
    co_return;
}

Treadle::DependentTask waitOnFlip() {
    //spawn new task that we depend on
    co_await doFlip();
}

template<typename... TASKS>
class When
{
public:
    explicit When(TASKS&&... tasks) 
        : count(sizeof...(tasks))
    {}
    uint32_t count;
};

int main()
{
    When<int, int, char, uint32_t> w(2,3,'q',5);

    std::cout << "Hello World!\n";

    auto task = LoopSync(1);

    std::cout << "task init done \n";

    while (!task.Done())
    {
        std::cout << "resuming from main\n";
        task.Resume();
        std::cout << "task step complete \n";
    }

    auto task2 = waitOnFlip();
    if (!task2.Done()) {
        task2.Resume();
    }
}