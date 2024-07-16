#pragma once
#include <mutex>
#include <unordered_map>

namespace Treadle
{
	//Slow thread-safe wrapper around std::unordered_map
	template <typename Key, typename Value>
	class ThreadSafeMap {
	public:

		Value& at(Key const& k) noexcept {
			std::lock_guard lock(m_mutex);
			return m_map.at(k);
		}

		void erase(Key const& k) noexcept {
			std::lock_guard lock(m_mutex);
			m_map.erase(k);
		}

		auto emplace(Key const& k, Value&& v) noexcept {
			std::lock_guard lock(m_mutex);
			return m_map.emplace(k, std::forward<Value>(v));
		}

	private:
		mutable std::mutex m_mutex;
		std::unordered_map<Key, Value> m_map;
	};
}