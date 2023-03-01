#include <atomic>
#include <immintrin.h>
#include <thread>
#include <cstdint>
#include "Locks.h"

namespace Locks
{
	ExpBackoffSpinLock::ExpBackoffSpinLock() noexcept
		: m_bLocked(false)
		, m_noPauseAttempts(8) // 8 is apparently empirically chosen, other values not so much
		, m_shortPauseAttempts(24)
		, m_longPauseAttempts(3000)
	{}

	ExpBackoffSpinLock::ExpBackoffSpinLock(uint8_t noPauseAttempts, uint8_t shortPauseAttempts, uint8_t longPauseAttempts) noexcept
		: m_bLocked(false)
		, m_noPauseAttempts(noPauseAttempts)
		, m_shortPauseAttempts(shortPauseAttempts)
		, m_longPauseAttempts(longPauseAttempts)
	{}

	void ExpBackoffSpinLock::lock()
	{
		for (uint32_t i = 0; i < m_noPauseAttempts; i++)
		{
			if (try_lock()) return;
		}

		for (uint32_t i = 0; i < m_shortPauseAttempts; i++)
		{
			if (try_lock()) return;
			_mm_pause();
		}

		while (true)
		{
			for (uint32_t i = 0; i < m_longPauseAttempts; i++)
			{
				if (try_lock()) return;

				//Apparently these are not loop-unrolled automatically, so doing it here
				_mm_pause();
				_mm_pause();
				_mm_pause();
				_mm_pause();
				_mm_pause();
				_mm_pause();
				_mm_pause();
				_mm_pause();
				_mm_pause();
				_mm_pause();
			}

			std::this_thread::yield();
		}
	}

	void ExpBackoffSpinLock::unlock() noexcept
	{
		m_bLocked.store(false, std::memory_order_release);
	}

	inline bool ExpBackoffSpinLock::try_lock() noexcept
	{
		return !(m_bLocked.load(std::memory_order_relaxed) || m_bLocked.exchange(true, std::memory_order_acquire));
	}

	SpinLock::SpinLock() noexcept
		: m_bLocked(false)
		, m_spinTimes(8 /*Empirically found constant as a default*/)
	{}

	SpinLock::SpinLock(uint32_t timesToSpin) noexcept
		: m_bLocked(false)
		, m_spinTimes(timesToSpin)
	{}

	void SpinLock::lock() noexcept {
		//probe first with the initial load
		// if that succeeds than attempt to lock
		for (uint32_t i = 0; !try_lock(); i++)
		{
			//if we spin for too long OS scheduler may incorrectly prioritize this threads execution
			// so we occassionally pause the cpu for some cycles
			// for more info see https://stackoverflow.com/questions/12894078/what-is-the-purpose-of-the-pause-instruction-in-x86/12904645
			if (i == m_spinTimes)
			{
				i = 0;
				_mm_pause();
			}
		}
	}

	void SpinLock::unlock() noexcept {
		m_bLocked.store(false, std::memory_order_release);
	}

	inline bool SpinLock::try_lock() noexcept {
		return !(m_bLocked.load(std::memory_order_relaxed) || m_bLocked.exchange(true, std::memory_order_acquire));
	}
}