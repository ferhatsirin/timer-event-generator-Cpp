#ifndef MYTIMER_H
#define MYTIMER_H
#include <list>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "ITimer.h"

using namespace std;

class MyTimer : public ITimer {
public:
    MyTimer();
    virtual ~MyTimer();
    
    // run the callback once at time point tp.
    virtual void registerTimer(const Timepoint& tp, const TTimerCallback& cb) override;
    // run the callback periodically forever. The first call will be executed as soon as this callback is registered.
    virtual void registerTimer(const Millisecs &period, const TTimerCallback& cb) override;
    // Run the callback periodically until time point tp. The first call will be executed as soon as this callback is
    //registered.
    virtual void registerTimer(const Timepoint& tp, const Millisecs& period, const TTimerCallback& cb) override;
    // Run the callback periodically. Before calling the callback every time, call the predicate first to check if the
    //termination criterion is satisfied. If the predicate returns false, stop calling the callback.
    virtual void registerTimer(const TPredicate& pred, const Millisecs& period, const TTimerCallback& cb) override;

private:

    struct Register {
        const TTimerCallback cb;
        Timepoint tp;
        const Millisecs period;
        const TPredicate pred;
        Register(const TTimerCallback& func, const Timepoint& time, const Millisecs& sec, const TPredicate& pr) : cb(func), tp(time), period(sec), pred(pr) {
        }
    };

    list<Register> record;
    thread th;
    mutex m;
    condition_variable cv;

private:
    void run();
    void addRecord(const Register& rec);
    static bool compare(const Register& rec1, const Register& rec2);

};

#endif /* MYTIMER_H */


