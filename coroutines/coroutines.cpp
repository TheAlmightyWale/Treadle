#include <iostream>
#include <coroutine>
#include <queue>
#include "DependentTask.h"
#include "MultiDependentTask.h"
#include "Task.h"
#include "DependedTask.h"

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

Treadle::DependedTask CreateTask(Treadle::Counter_t* pCounter, std::coroutine_handle<>* pContinuation) noexcept {
    co_return;
}

void SplitTasks(std::vector<Treadle::DependedTask>&mem, std::vector<Treadle::MultiDependentTask>& multiMem) {
    std::cout << "Splitting tasks\n";
    multiMem.emplace_back(2);

    mem.emplace_back(CreateTask(&multiMem.back().waitCount_, &multiMem.back().continuation_));
    mem.emplace_back(CreateTask(&multiMem.back().waitCount_, &multiMem.back().continuation_));
}

Task DoSomething(std::vector<Treadle::DependedTask>& basic, std::vector<Treadle::MultiDependentTask>& multi) noexcept {
    std::cout << "doing something multi\n";
    
    SplitTasks(basic, multi);

    std::cout << "inner multi deps run \n";

    for (auto& t : basic) {
        if (!t.Done()) {
            t.Resume();
        }
    }

    co_await multi[0];

    std::cout << "multi completed \n";
}



void RunTasks() {

}

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

    //auto task2 = waitOnFlip();
    //if (!task2.Done()) {
    //    task2.Resume();
    //}

    std::vector<Treadle::DependedTask> basic;
    std::vector<Treadle::MultiDependentTask> multi;
    auto ds = DoSomething(basic, multi);
    ds.Resume();

    std::cout << "first multi deps run \n";

    for (auto& t : basic) {
        if (!t.Done()) {
            t.Resume();
        }
    }

    std::cout << "second multi deps run \n";

    for (auto& t : basic) {
        if (!t.Done()) {
            t.Resume();
        }
    }


}