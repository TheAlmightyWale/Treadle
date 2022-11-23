#include <functional>
#include <iostream>
#include <thread>

export module WorkJob;

namespace Treadle
{

export class Job {
	public:
		Job(std::function<void(void)> job);
		void Execute();

	private:
		std::function<void(void)> m_fn;
};

Job::Job(std::function<void(void)> job)
	: m_fn(job)
{}

void Job::Execute()
{
	std::cout << "Executing Job on thread: " << std::this_thread::get_id();
	m_fn();
}

}