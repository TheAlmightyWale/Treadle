#pragma once
//#include <ranges>
//#include "TaskTraits.hpp"
//
//namespace Treadle2
//{
//
//	template <IsPollable T>
//	inline void SyncWait(T const& task) {
//		while (!task.Done()) {
//			//nothing
//			;;
//		}
//	}
//	
//	template <IsPollable T>
//	inline void SyncWaitAll(std::ranges::input_range<T> const& tasks) {
//		for (auto const& task : tasks) {
//			SyncWait(task);
//		}
//	}
//}
//
