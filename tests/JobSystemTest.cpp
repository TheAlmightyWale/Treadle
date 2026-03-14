#include "gtest/gtest.h"
#include <Treadle/public/JobSystem.h>
#include <Treadle/public/Task.hpp>
#include <Treadle/public/Wait.hpp>
#include "TestUtils.hpp"

namespace TreadleTest
{
    constexpr uint32_t k_numTasks = 1;
    Treadle::Task<void> CreateTask() {
        co_return;
    }

    Treadle::Task<int> CreateNumTask() {
        co_return 2;
    }

    TEST(JobSystemTests, LaunchesJobs)
    {
        Treadle::JobSystem js(1);
        auto task = CreateTask();

        js.AddJob(task.GetCoroutine());

        js.Start();

        while (!js.GetQueue().empty()) {
            //spin
        }

        js.JoinAll();

        EXPECT_TRUE(task.Done());
    }

    TEST(JobSystemTests, AcceptsMultipleJobTypes)
    {
        auto t1 = CreateTask();
        auto t2 = CreateNumTask();

        Treadle::JobSystem js(1);

        js.AddJob(t1.GetCoroutine());
        js.AddJob(t2.GetCoroutine());

        js.Start();

        while (!js.GetQueue().empty()) {
            //spin
        }

        js.JoinAll();

        EXPECT_TRUE(t1.Done());
        EXPECT_TRUE(t2.Done());
    }

    TEST(JobSystemTests, HandlesDependantJobs)
    {
        auto t1 = CreateTask();
        auto t2 = CreateNumTask();
        auto t3 = Treadle::Wait(t1, t2);

        Treadle::JobSystem js(1);

        js.AddJob(t1.GetCoroutine());
        js.AddJob(t2.GetCoroutine());

        js.Start();

        while (!js.GetQueue().empty()) {
            //spin
        }

        js.JoinAll();

        EXPECT_TRUE(t1.Done());
        EXPECT_TRUE(t2.Done());
        EXPECT_TRUE(t3.Done());
    }

    Treadle::Task<void> DeepTask3(bool& task3Complete){
        task3Complete = true;
        co_return;
    }

    Treadle::Task<void> DeepTask2(bool& task2Complete, bool& task3Complete){
        co_await DeepTask3(task3Complete);
        task2Complete = true;
    }

    Treadle::Task<void> DeepTask1(bool& task1Complete, bool& task2Complete, bool& task3Complete){
        co_await DeepTask2(task2Complete, task3Complete);
        task1Complete = true;
    }

    TEST(JobSystemTests, HandlesDeepDependencyChain){
        //only schedule top level task
        bool task1Complete = false;
        bool task2Complete = false;
        bool task3Complete = false;

        auto t1 = DeepTask1(task1Complete, task2Complete, task3Complete);
        Treadle::JobSystem js(1);
        js.AddJob(t1.GetCoroutine());

        //Use job system scheduler to schedule tasks it waits on.
        Treadle::IScheduler::Get().SetScheduleFn([&js](Job job){
            js.AddJob(job);
        });

        js.Start();

        while (!t1.Done()) {
            //spin
        }

        js.JoinAll();

        //verify follow on tasks were scheduled as well
        EXPECT_TRUE(task1Complete);
        EXPECT_TRUE(task2Complete);
        EXPECT_TRUE(task3Complete);
    }
}
