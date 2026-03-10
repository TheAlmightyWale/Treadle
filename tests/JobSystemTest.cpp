#include "gtest/gtest.h"
#include "JobSystem.h"
#include "Task.hpp"
#include "Wait.hpp"

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

        Treadle::Job job(task);
        js.AddJob(std::move(job));

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

        js.AddJob({ t1 });
        js.AddJob({ t2 });

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

        js.AddJob({ t1 });
        js.AddJob({ t2 });

        js.Start();

        while (!js.GetQueue().empty()) {
            //spin
        }

        js.JoinAll();

        EXPECT_TRUE(t1.Done());
        EXPECT_TRUE(t2.Done());
        EXPECT_TRUE(t3.Done());
    }
}
