#pragma once
#include <functional>
#include <cassert>
#include "Job.h"

namespace Treadle
{
using ScheduleJobFn = std::function<void(Job)>;

namespace Detail{
    static void DefaultScheduleFn(Job)
    {
        assert(false && "Trying to schedule job without having set scheduling function");
    }
}

//Singleton with overrideable schdule function
class IScheduler{
public:
    static IScheduler& Get(){
        static IScheduler sched;
        return sched;
    }

    void Schedule(Job job){
        schedFn_(job);
    }

    void SetScheduleFn(ScheduleJobFn const& fn){
        schedFn_ = fn;
    }

protected:
    IScheduler() : schedFn_{Detail::DefaultScheduleFn}
    {}

private:
    ScheduleJobFn schedFn_;
};


}