#include "gtest/gtest.h"
#include "../src/MultiDependentTask.h"
#include "../src/AtomicCounter.h"
#include "../src/DependedTask.h"
#include "../src/Task.h"

namespace {
    Treadle::DependedTask CreateDependedTask(Treadle::Counter_t* pCounter, std::coroutine_handle<>* pContinuation) noexcept {
        co_return;
    }

    void SplitTasks(std::vector<Treadle::DependedTask>& mem, std::vector<Treadle::MultiDependentTask>& multiMem) {
        std::cout << "Splitting tasks\n";
        multiMem.emplace_back(2);

        mem.emplace_back(CreateDependedTask(&multiMem.back().waitCount_, &multiMem.back().continuation_));
        mem.emplace_back(CreateDependedTask(&multiMem.back().waitCount_, &multiMem.back().continuation_));
    }

    Treadle::Task<Treadle::LoggedPromise> WaitOnMultipleTasks(std::vector<Treadle::DependedTask>& basic, std::vector<Treadle::MultiDependentTask>& multi) noexcept {
        std::cout << "doing something multi\n";

        SplitTasks(basic, multi);
        co_await multi[0];

        std::cout << "multi completed \n";
    }
}

TEST(MultiDependentTask, ResumesWhenDependentsComplete) {
    //Arrange
    std::vector<Treadle::DependedTask> depended;
    std::vector<Treadle::MultiDependentTask> dependent;

    //Act
    auto task = WaitOnMultipleTasks(depended, dependent);
    task.Resume();

    EXPECT_TRUE(dependent[0].waitCount_ > 0);
    EXPECT_FALSE(task.Done());

    for (auto& t : depended) {
        if (!t.Done()) {
            t.Resume();
        }
    }

    //Assert
    EXPECT_TRUE(task.Done());
    EXPECT_EQ(dependent[0].waitCount_, 0);
}

struct TestPromise {
    struct TestFinalizer {
        bool await_ready() const noexcept { return false; }
        void await_resume() const noexcept {}
        std::coroutine_handle<> await_suspend(std::coroutine_handle<TestPromise> h) const noexcept
        {
            h.promise().m_bFinalizerCalled = true;
            return std::noop_coroutine();
        }
    };

    TestPromise(bool& bFinalizerCalled) : m_bFinalizerCalled(bFinalizerCalled) {}

    Treadle::Task<TestPromise> get_return_object() noexcept {
        return Treadle::Task<TestPromise>{ std::coroutine_handle<TestPromise>::from_promise(*this) };
    }

    std::suspend_always initial_suspend() const {
        return {};
    }
    TestFinalizer final_suspend() const noexcept {
        return {};
    }

    void unhandled_exception() const {
        std::terminate();
    }

    void return_void() const noexcept {
    }
    bool& m_bFinalizerCalled;
};


Treadle::Task<TestPromise> TestTask(bool& bFinalizerCalled) {
    co_return;
}

TEST(Task, CallsAssignedFinalizer) {
    //Arrange
    bool bFinalizerCalled = false;
    auto task = TestTask(bFinalizerCalled);

    //Act
    task.Resume();

    //Assert
    EXPECT_TRUE(bFinalizerCalled);
}