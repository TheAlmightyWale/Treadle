#include <cstdint>
#include <iostream>
#include <functional>
#include <thread>
#include <syncstream>

import PrimeNumbers;
import JobSystem;
import JobQueue;
import WorkJob;

auto printIdLambda = [](int id)
{
	std::osyncstream{ std::cout } << "Id: " << id << std::endl;
};

auto main(int argc, char* argv[]) -> int
{
	std::osyncstream{ std::cout } << "Starting Job System" << std::endl;

	Treadle::JobSystem jobSystem(8);
	Treadle::JobQueue& queue = jobSystem.GetQueue();
	
	std::osyncstream{ std::cout } << "Pushing job to queue" << std::endl;

	auto func = std::bind(PrimeNumbers::Find, 50);
	Treadle::Job calcPrime(func);
	queue.Push(calcPrime);

	for (uint32_t i = 0; i < 100; ++i)
	{
		auto func = std::bind(printIdLambda, i);
		Treadle::Job printId(func);
		queue.Push(printId);
	}

	std::osyncstream{ std::cout } << "Waiting on Job completion" << std::endl;
}