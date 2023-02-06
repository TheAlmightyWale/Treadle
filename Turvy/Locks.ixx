module;
#include <atomic>
#include <immintrin.h>
#include <thread>

export module Locks;

//TODO Have benchmark functions that can be run to propagate better values for spinlock attempts
// based on the running hardware

export namespace Locks
{
	// Based on Timur Doumlers implementation in https://timur.audio/using-locks-in-real-time-audio-processing-safely
	// This spinlock will attempt to gain the lock m_noPauseAttempts times before moving on
	// It will then spin m_shortPauseAttempts times with a pause in between them
	// It will then spin m_longPauseAttempts times with a 10x longer pause before yielding
	// After returning from the yield we will spin with long pauses until yielding again
	class ExpBackoffSpinLock {
	public: 
		ExpBackoffSpinLock()
			: m_bLocked(false)
			, m_noPauseAttempts(8) // 8 is apparently empirically chosen, other values not so much
			, m_shortPauseAttempts(24)
			, m_longPauseAttempts(3000)
		{}

		ExpBackoffSpinLock(uint8_t noPauseAttempts, uint8_t shortPauseAttempts, uint8_t longPauseAttempts)
			: m_bLocked(false)
			, m_noPauseAttempts(noPauseAttempts)
			, m_shortPauseAttempts(shortPauseAttempts)
			, m_longPauseAttempts(longPauseAttempts)
		{}

		void lock() 
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

		void unlock()
		{
			m_bLocked.store(false, std::memory_order_release);
		}

	private:

		inline bool try_lock()
		{
			return !(m_bLocked.load(std::memory_order_relaxed) || m_bLocked.exchange(true, std::memory_order_acquire));
		}

		std::atomic<bool> m_bLocked;
		uint32_t const m_noPauseAttempts;
		uint32_t const m_shortPauseAttempts;
		uint32_t const m_longPauseAttempts;
	};

	//based on Fedor Pikus' implementation in https://www.youtube.com/watch?app=desktop&v=rmGJc9PXpuE

	class SpinLock {
	public:
		SpinLock()
			: m_bLocked(false)
			, m_spinTimes(8 /*Empirically found constant as a default*/)
		{}

		SpinLock(uint32_t timesToSpin)
			: m_bLocked(false)
			, m_spinTimes(timesToSpin)
		{}

		void lock() noexcept {
			//probe first with the initial load
			// if that succeeds than attempt to lock
			for(uint32_t i = 0; !try_lock(); i++)
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

		void unlock() noexcept {
			m_bLocked.store(false, std::memory_order_release);
		}

	private:
		inline bool try_lock() noexcept {
			return !(m_bLocked.load(std::memory_order_relaxed) || m_bLocked.exchange(true, std::memory_order_acquire));
		}

		std::atomic<bool> m_bLocked;
		uint32_t const m_spinTimes;
	};
}