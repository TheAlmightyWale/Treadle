#include <functional>
#include <iostream>
#include <syncstream>
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
	m_fn();
}

}