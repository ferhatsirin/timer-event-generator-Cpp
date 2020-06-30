#ifndef ITIMER_H
#define ITIMER_H

#include <chrono>
#include <functional>


using CLOCK = std::chrono::high_resolution_clock;
using TTimerCallback = std::function<void()>;
using Millisecs = std::chrono::milliseconds;
using Timepoint =CLOCK::time_point;
using TPredicate = std::function<bool()>;

class ITimer {
public:
    ITimer();
    virtual ~ITimer();
    
    virtual void registerTimer(const Timepoint& tp,const TTimerCallback& cb)=0;
    virtual void registerTimer(const Millisecs &period, const TTimerCallback& cb)=0;
    virtual void registerTimer(const Timepoint& tp,const Millisecs& period,const TTimerCallback& cb)=0;
    virtual void registerTimer(const TPredicate& pred, const Millisecs& period, const TTimerCallback& cb )=0;
private:

};

#endif /* ITIMER_H */


