#pragma once
#include <atomic>
#include <thread>
#include <bitset>

#include "OsDef.h"

namespace Treadle{
	inline bool SetWorkerThreadAffinity(int coreId, std::jthread& thread)
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
	inline auto CreateAndStartThread(int coreId, std::string const& name, FunctionType&& func, ArgTypes&& ... args) noexcept -> std::jthread* {
		auto threadBody = [&](std::stop_token stopToken) {
				std::forward<FunctionType>(func)(stopToken, (std::forward<ArgTypes>(args))...);
			};

		auto* pThread = new std::jthread(threadBody);

		if (coreId >= 0 && !SetWorkerThreadAffinity(coreId, *pThread)) {
			std::cerr << "Failed to set core affinity for " << name << " " << pThread->get_id() << " to " << coreId << "\n";
			pThread->get_stop_source().request_stop();
			pThread->join();
			delete pThread;
			pThread = nullptr;

			return nullptr;
		}

		//std::cout << "Set core affinity for " << name << " " << pThread->get_id() << " to " << coreId << "\n";

		return pThread;
	}
}

