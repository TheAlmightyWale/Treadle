#pragma once
#include <iostream>
#include <atomic>
#include <thread>
#include <bitset>

#include "WindowsDef.h"
#include "LinuxDef.h"

inline bool SetWorkerThreadAffinity(int coreId, std::thread& thread)
{
#ifdef _WIN64
	std::bitset<64> threadAffinityBitMask = 1i64 << coreId;
	DWORD_PTR affinityMask = threadAffinityBitMask.to_ulong();
	DWORD_PTR res = SetThreadAffinityMask(thread.native_handle(), affinityMask);
	return res;
#elif __linux__
	cpu_set_t cpuset;
	CPU_ZEOR(&cpuset);
	CPU_SET(coreId, &cpuset);

	return pthread_setaffinity_np(thread.native_handle(), sizeof(cpu_set_t), &cpuset);
#else
	static_assert(false, "Implmentation for other OS' required");
#endif 

}

template<typename FunctionType, typename... ArgTypes>
inline auto CreateAndStartThread(int coreId, std::string const& name, FunctionType&& func, ArgTypes&& ... args) noexcept {
	std::atomic_bool bRunning(false);
	std::atomic_bool bFailed(false);

	//does this work? or is thread non-existent at this point?
	auto threadBody = [&] {
		while (!bRunning) {
			if (bFailed) {
				std::cerr << "Failure detected while initilizing thread " << std::this_thread::get_id() << " exiting\n";
				return;
			}
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(1s);
		}
		std::forward<FunctionType>(func)((std::forward<ArgTypes>(args))...);
	};

	auto pThread = new std::thread(threadBody);

	if (coreId >= 0 && !SetWorkerThreadAffinity(coreId, *pThread)) {
		std::cerr << "Failed to set core affinity for " << name << " " << pThread->get_id() << " to " << coreId << "\n";
		bFailed = true;
		pThread->join();
		delete pThread;
		pThread = nullptr;
	}

	std::cout << "Set core affinity for " << name << " " << pThread->get_id() << " to " << coreId << "\n";
	bRunning = true;

	return pThread;
}

