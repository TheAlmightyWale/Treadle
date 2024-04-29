#include <benchmark/benchmark.h>
#include <memory>
#include <chrono>
#include <thread>
#include <functional>
#include "JobSystem.h"
#include "JobQueue.h"
#include "WorkJob.h"

import PrimeNumbers;

auto smallJobLambda = []()
{
	//Find a small prime
	PrimeNumbers::Find(50);
};

auto shesAPrettyBigJobMateLambda = []()
{
	//Find a big prime
	PrimeNumbers::Find(1000);
};

//Performance characteristics we wish to test:
// Thread overhead in removing from queue (more threads more overhead, but is it bad?)
// Test for worst case with lots of very small jobs
// Test larger jobs
// Test mix of longer running and shorter running jobs
//Base of single thread vs many threads
static void BM_JobSystem(benchmark::State& state)
{
	for (auto _ : state)
	{
		if (state.thread_index() == 0)
		{
			state.PauseTiming();
			Treadle::JobSystem jobSystem(static_cast<uint32_t>(state.range(0)));
			Treadle::JobQueue& queue = jobSystem.GetQueue();
			jobSystem.Pause();

			//Queue jobs based on range input
			if (state.range(1) == 1)
			{
				for (uint32_t i = 0; i < state.range(2); i++)
				{
					auto func = std::bind(smallJobLambda);
					queue.Push({ func });
				}
			}
			else if (state.range(1) == 2)
			{
				for (uint32_t i = 0; i < state.range(2); i++)
				{
					auto func = std::bind(shesAPrettyBigJobMateLambda);
					queue.Push({ func });
				}
			}
			else
			{
				for (uint32_t i = 0; i < state.range(2); i++)
				{
					uint32_t mod = i % 4;
					if (mod == 0)
					{
						auto func = std::bind(shesAPrettyBigJobMateLambda);
						queue.Push({ func });
					}
					else
					{
						auto func = std::bind(smallJobLambda);
						queue.Push({ func });
					}
				}
			}
			state.ResumeTiming();
			jobSystem.Resume();
		}
	}
} 

BENCHMARK(BM_JobSystem)->ArgsProduct({
	benchmark::CreateRange(1,16,2), // Threads used
	benchmark::CreateDenseRange(1,3,1), // Job sizes (1 - small, 2 - large, 3 - mixed small and large)
	benchmark::CreateRange(100, 500, 100) // Jobs queued
	})->UseRealTime()->Threads(16);

BENCHMARK_MAIN();