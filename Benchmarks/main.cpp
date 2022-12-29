#include <benchmark/benchmark.h>
#include <memory>
#include <chrono>
#include <thread>
#include <functional>

import JobSystem;

auto smallJobLambda = []()
{
	//Delay for very small amount of time
	std::this_thread::sleep_for(std::chrono::microseconds(10));
};

auto shesAPrettyBigJobMateLambda = []()
{
	//Delay for a larger amount of time
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
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
		state.PauseTiming();
		Treadle::JobSystem jobSystem(state.range(0));
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

BENCHMARK(BM_JobSystem)->ArgsProduct({
	benchmark::CreateRange(1,32,2), // Threads used
	benchmark::CreateDenseRange(1,3,1), // Job sizes (1 - small, 2 - large, 3 - mixed small and large)
	benchmark::CreateRange(1000, 5000, 1000) // Jobs queued
	})->UseRealTime();


BENCHMARK_MAIN();