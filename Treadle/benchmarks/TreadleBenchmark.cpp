#include "gtest/gtest.h"
#include <nanobench.h>

#include "../src/JobSystem.h"
#include "../src/Task.h"
#include "../src/MpmcQueue.hpp"
#include "../src/BasicPromise.h"

namespace {

    bool IsPrime(uint64_t number)
    {
        //Purposely inefficient prime finding algorithm
        uint64_t temp = 0;
        for (uint64_t i = 2; i < number - 1; ++i)
        {
            if (number % i == 0)
            {
                return false;
            }
        }
        return true;
    }

    uint64_t FindPrime(uint32_t nthPrime)
    {
        uint64_t prime = 0;

        uint64_t checkedNum = 2;
        uint64_t numPrimesFound = 0;
        while (prime == 0)
        {
            if (IsPrime(checkedNum))
            {
                ++numPrimesFound;
            }

            if (numPrimesFound == nthPrime)
            {
                prime = checkedNum;
            }

            ++checkedNum;
        }

        return prime;
    }

    uint64_t SmallPrime() {
        return FindPrime(300);
    }

    uint64_t LargePrime() {
        return FindPrime(800);
    }
}

namespace Bench = ankerl::nanobench;

//TEST(TreadleBenchmark, SmallPrimeBaseline) {
//    Bench::Bench().minEpochIterations(1000).run("SmallPrimeBaseline", [] {
//        Bench::doNotOptimizeAway(SmallPrime());
//        });
//}
//
//TEST(TreadleBenchmark, LargePrimeBaseline) {
//    Bench::Bench().minEpochIterations(100).run("LargePrimeBaseline", [] {
//        Bench::doNotOptimizeAway(LargePrime());
//        });
//}


constexpr uint32_t k_numTasks = 100;
Treadle::Task<Treadle::BasicPromise> CreateSmallPrimeTask() {
    Bench::doNotOptimizeAway(SmallPrime());
    co_return;
}

TEST(TreadleBenchmark, SmallPrimeCalculations) {
    //start system and time
    Bench::Bench().minEpochIterations(100).run("SmallPrimeCalculations", [] {
        Treadle::JobSystem js(8);

        //add jobs
        for (uint32_t i = 0; i < k_numTasks; ++i) {
            js.AddJob(CreateSmallPrimeTask());
        }

        js.Start();

        while (!js.GetQueue().empty()) {
            //spin
        }

        js.JoinAll();
    });
}
