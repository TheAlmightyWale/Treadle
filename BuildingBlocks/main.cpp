#include "Thread.hpp"

auto dummyFunction(int a, int b, bool sleep) {
	std::cout << "dummyfunction " << a << ", " << b << "\n";
	std::cout << "dummyFunction output= " << a + b << "\n";

	if (sleep) {
		std::cout << "dummyFunction sleeping... \n";

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(5s);
	}

	std::cout << "dummy function done. \n";
}

int main() {
	auto* t1 = CreateAndStartThread(-1, "t1", dummyFunction, 12, 21, false);
	auto* t2 = CreateAndStartThread(1, "t2", dummyFunction, 15, 51, true);

	std::cout << "main waiting for threads to be done \n";
	t1->join();
	t2->join();

	std::cout << "main exiting \n";

	return 0;
}