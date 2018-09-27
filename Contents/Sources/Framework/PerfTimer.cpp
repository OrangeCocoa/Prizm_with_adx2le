
#include<chrono>
#include"PerfTimer.h"

namespace Prizm
{
	using Time = std::chrono::time_point<std::chrono::system_clock>;
	using Duration = std::chrono::duration<float>;

	class PerfTimer::Impl
	{
	public:
		Time		base_time_,
					prev_time_,
					curr_time_,
					start_time_,
					stop_time_;
		Duration	paused_time_,
					dt_;
		bool		is_stopped_;

		Impl() : is_stopped_(false){}
	};

	PerfTimer::PerfTimer() : impl_(std::make_unique<Impl>()){}
	PerfTimer::~PerfTimer() = default;

	float PerfTimer::Tick()
	{
		if (impl_->is_stopped_)
		{
			impl_->dt_ = Duration::zero();
			return impl_->dt_.count();
		}

		impl_->curr_time_ = std::chrono::system_clock::now();
		impl_->dt_ = impl_->curr_time_ - impl_->prev_time_;

		impl_->prev_time_ = impl_->curr_time_;
		impl_->dt_ = impl_->dt_.count() < 0.0f ? impl_->dt_.zero() : impl_->dt_;

		return impl_->dt_.count();
	}

	void PerfTimer::Start()
	{
		if (impl_->is_stopped_)
		{
			impl_->paused_time_ = impl_->start_time_ - impl_->stop_time_;
			impl_->prev_time_ = std::chrono::system_clock::now();
			impl_->is_stopped_ = false;
		}
		Tick();
	}

	void PerfTimer::Stop()
	{
		Tick();
		if (!impl_->is_stopped_)
		{
			impl_->stop_time_ = std::chrono::system_clock::now();
			impl_->is_stopped_ = true;
		}
	}

	float PerfTimer::DeltaTime() const
	{
		return impl_->dt_.count();
	}

	float PerfTimer::TotalTime() const
	{
		Duration total_time = Duration::zero();

		if (impl_->is_stopped_) total_time = (impl_->stop_time_ - impl_->base_time_) - impl_->paused_time_;
		else total_time = (impl_->curr_time_ - impl_->base_time_) - impl_->paused_time_;

		return total_time.count();
	}

	void PerfTimer::Reset()
	{
		impl_->base_time_ = impl_->prev_time_ = std::chrono::system_clock::now();
		impl_->is_stopped_ = true;
		impl_->dt_ = Duration::zero();
	}
}
