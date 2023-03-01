#include <cstdint>
#include <iostream>
#include <functional>
#include <thread>
#include <syncstream>
#include <random>

#include "JobSystem.h"
#include "JobQueue.h"
#include "WorkJob.h"

import PrimeNumbers;

auto main(int argc, char* argv[]) -> int
{
	std::osyncstream{ std::cout } << "Starting Job System" << std::endl;

	{
		Treadle::JobSystem jobSystem(8);
		Treadle::JobQueue& queue = jobSystem.GetQueue();
		jobSystem.Pause();

		std::osyncstream{ std::cout } << "Pushing job to queue" << std::endl;

		std::random_device rd; //obtain random number from hardware
		std::mt19937 generator(rd());
		std::uniform_int_distribution distribution(3000, 3200);// range is inclusive

		for (uint32_t i = 0; i < 100; ++i)
		{
			auto func = std::bind(PrimeNumbers::Find, distribution(generator));
			Treadle::Job calcPrime(func);
			queue.Push(calcPrime);

			std::osyncstream{ std::cout } << "Pushing job " << i << " to queue" << std::endl;
		}

		std::osyncstream{ std::cout } << "Waiting on Job completion" << std::endl;
		jobSystem.Resume();
	}
}