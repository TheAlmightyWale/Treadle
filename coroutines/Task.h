#pragma once

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

        int m_id = getGid();
    };

    Task(Task&& t) noexcept
        : coro_(std::exchange(t.coro_, {}))
    {
        std::cout << "Creating Task: " << m_id << "\n";
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