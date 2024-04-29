#pragma once
#include <stop_token>
#include <optional>
#include "WorkJob.h"

namespace Treadle
{
	class WorkStealingWorker
	{
	public:
		WorkStealingWorker() noexcept;

		void Run(std::stop_token stopToken);
		void Pause();
		void Resume();

	protected:
		std::optional<Job> GetNextJob();

		bool m_bIsPaused;
		uint32_t m_id;
	};
}