#include "MyTimer.h"
#include <iostream>

using namespace std;

MyTimer::MyTimer() {
    th = thread(&MyTimer::run, this);
}

MyTimer::~MyTimer() {
    th.join();
}

/**
 * thread runs this method. This method runs the callback of records when their time is up.
 */
void MyTimer::run() {

    while (true) {
        unique_lock<mutex> lk(m);
        
        Timepoint timeout;
        if (0 < record.size()) {
            timeout = record.front().tp;
        } else {
            timeout = Timepoint::max();
        }
        bool cond = false;
        
        while (!cond) {  // if timeout is reached but predicate is still false, keep waiting
            cond = cv.wait_until(lk, timeout, [&]()->bool {
                if (record.size() == 0) {
                    timeout = Timepoint::max(); // wait until list is added
                    return false;
                }
                if (CLOCK::now() < record.front().tp) { // wait until timeout is reached
                    timeout = record.front().tp; 
                    return false;
                }
                return true;
            });
        }
        
        Register rec = record.front();
        record.pop_front();
        lk.unlock();

        if (rec.pred()) { // call predicate
            Timepoint start =CLOCK::now();
            rec.cb();   // call callback
            if(rec.tp+Millisecs(5) < start){  // if callback is called 5 millisecond late then print deadline error
                cout<<"Deadline error for that callback."<<endl;
            }
            if (rec.period.count() != 0) {
                rec.tp = CLOCK::now() + rec.period;  // next call will wait for period 
                addRecord(rec);
            }
        }
    }
}


/**
 * add the record to the list. thread safe function.
 * Sort the list every time after adding is done.
 * @param rec record
 */
void MyTimer::addRecord(const Register& rec) {

    unique_lock<mutex> lk(m);
    record.push_back(rec);
    record.sort(compare);
    lk.unlock();
    cv.notify_one();
}

/**
 * compare two record by comparing thir time point
 * @param rec1 first record
 * @param rec2 second record
 * @return if first record is smaller than second one then return true, otherwise false
 */
bool MyTimer::compare(const Register& rec1, const Register& rec2) {
    if (rec1.tp < rec2.tp) {
        return true;
    } else {
        return false;
    }
}

// run the callback once at time point tp.
void MyTimer::registerTimer(const Timepoint& tp, const TTimerCallback& cb) {

    Register reg(cb, tp, Millisecs(0), []() -> bool {
        return true;
    });

    addRecord(reg);
}

// run the callback periodically forever. The first call will be executed as soon as this callback is registered.
void MyTimer::registerTimer(const Millisecs &period, const TTimerCallback& cb) {

    Register reg(cb, CLOCK::now(), period, []() -> bool {
        return true;
    });

    addRecord(reg);
}

// Run the callback periodically until time point tp. The first call will be executed as soon as this callback is
//registered.
void MyTimer::registerTimer(const Timepoint& tp, const Millisecs& period, const TTimerCallback& cb) {

    Register reg(cb, CLOCK::now(), period, [tp]() -> bool {
        if (CLOCK::now() < tp) {
            return true;
        } else {
            return false;
        }
    });

    addRecord(reg);

}

// Run the callback periodically. Before calling the callback every time, call the predicate first to check if the
//termination criterion is satisfied. If the predicate returns false, stop calling the callback.
void MyTimer::registerTimer(const TPredicate& pred, const Millisecs& period, const TTimerCallback& cb) {

    Register reg(cb, CLOCK::now(), period, pred);

    addRecord(reg);
}




