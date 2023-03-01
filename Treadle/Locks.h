#pragma once
#include <cstdint>
#include <memory>

//TODO Have benchmark functions that can be run to propagate better values for spinlock attempts
// based on the running hardware
namespace Locks
{
	// Based on Timur Doumlers implementation in https://timur.audio/using-locks-in-real-time-audio-processing-safely
	// This spinlock will attempt to gain the lock m_noPauseAttempts times before moving on
	// It will then spin m_shortPauseAttempts times with a pause in between them
	// It will then spin m_longPauseAttempts times with a 10x longer pause before yielding
	// After returning from the yield we will spin with long pauses until yielding again
	class ExpBackoffSpinLock {
	public:
		ExpBackoffSpinLock() noexcept;
		ExpBackoffSpinLock(uint8_t noPauseAttempts, uint8_t shortPauseAttempts, uint8_t longPauseAttempts) noexcept;

		void lock();
		void unlock() noexcept;

	private:
		inline bool try_lock() noexcept;

		std::atomic<bool> m_bLocked;
		uint32_t const m_noPauseAttempts;
		uint32_t const m_shortPauseAttempts;
		uint32_t const m_longPauseAttempts;
	};

	//based on Fedor Pikus' implementation in https://www.youtube.com/watch?app=desktop&v=rmGJc9PXpuE
	class SpinLock {
	public:
		SpinLock() noexcept;
		SpinLock(uint32_t timesToSpin) noexcept;

		void lock() noexcept;
		void unlock() noexcept;

	private:
		inline bool try_lock() noexcept;

		std::atomic<bool> m_bLocked;
		uint32_t const m_spinTimes;
	};
}