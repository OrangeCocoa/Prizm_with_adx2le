#pragma once

#include<memory>

namespace Prizm
{
	class PerfTimer
	{
	private:
		class Impl;
		std::unique_ptr<Impl> impl_;

	public:
		PerfTimer();
		~PerfTimer();

		float Tick();
		void Start();
		void Stop();
		float DeltaTime() const;
		float TotalTime() const;
		void Reset();
	};
}