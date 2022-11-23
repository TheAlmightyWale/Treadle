#include <cstdint>
#include <iostream>
#include <functional>
#include <thread>

import PrimeNumbers;
import JobSystem;
import JobQueue;
import WorkJob;

auto main(int argc, char* argv[]) -> int
{
	std::cout << "Starting Job System" << std::endl;

	Treadle::JobSystem jobSystem(1);
	Treadle::JobQueue& queue = jobSystem.GetQueue();
	
	std::cout << "Pushing job to queue" << std::endl;

	auto func = std::bind(PrimeNumbers::Find, 10);
	Treadle::Job calcPrime(func);
	queue.Push(calcPrime);

	std::cout << "Waiting on Job completion" << std::endl;
	jobSystem.Stop();
}