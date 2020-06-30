#include <cstdlib>
#include <chrono>
#include <iostream>
#include <functional>
#include <mutex>
#include <thread>
#include <string>
#include "MyTimer.h"

using namespace std;

// program starting time
const Timepoint startTime = CLOCK::now(); 

/**
 * return time in string format that contains millisecond, microsecond and nanosecond 
 * @param tp time to print if not given, then current time is used
 * @return time in string format
 */
string getTime(Timepoint tp = CLOCK::now()) {

    Timepoint now = tp - startTime.time_since_epoch();
    CLOCK::duration nanoSec = now.time_since_epoch() - chrono::duration_cast<chrono::seconds>(now.time_since_epoch());
    Millisecs milli = chrono::duration_cast<Millisecs>(nanoSec);
    nanoSec = nanoSec - milli;
    chrono::microseconds micro = chrono::duration_cast<chrono::microseconds>(nanoSec);
    nanoSec = nanoSec - micro;

    string time = to_string(milli.count()) + ":" + to_string(micro.count()) + ":" + to_string(nanoSec.count());
    
    return time;
}

// mutex for print operation
mutex m;
/**
 * only one thread can print at a time
 */
void safePrint(string str) { 
    lock_guard<mutex> lock(m);
    cout << str << endl;
}

void funA() {

    safePrint("funA is called at time " + getTime());
}

void funB() {

    safePrint("funB is called at time " + getTime());
}

void funC() {
    safePrint("funC is called at time " + getTime());
}

void funD() {
    safePrint("funD is called at time " + getTime());
}

void funE() {
    safePrint("funE is called at time " + getTime());
}


int main(int argc, char** argv) {

    MyTimer timer;

    safePrint("Time format ms:us:ns");
    safePrint("Program started at : " + getTime());

    Timepoint tp1 = CLOCK::now() + Millisecs(15);
    safePrint("FunA will be called at " + getTime(tp1));
    timer.registerTimer(tp1, funA);

    Timepoint tp2 = CLOCK::now() + Millisecs(3);
    safePrint("FunA will be called at " + getTime(tp2));
    timer.registerTimer(tp2, funA);

    Timepoint tp3 = CLOCK::now() + Millisecs(29);
    safePrint("FunA will be called at " + getTime(tp3));
    timer.registerTimer(tp3, funA);

    Millisecs p1(3);
    safePrint("FunB will be called with period " + to_string(p1.count()) + " ms");
    timer.registerTimer(p1, funB);

    Timepoint tp4 = CLOCK::now() + Millisecs(20);
    Millisecs p2(1);
    safePrint("FunC will be called with period " + to_string(p2.count()) + " ms until " + getTime(tp4));
    timer.registerTimer(tp4, p2, funC);

    Timepoint tp5 = CLOCK::now() + Millisecs(50);
    Millisecs p3(5);
    safePrint("FunD will be called with period " + to_string(p3.count()) + " ms until predicate returns false. Predicate will return false when time is " + getTime(tp5));
    timer.registerTimer([tp5]()->bool {
        if (CLOCK::now() < tp5) {
            return true;
        } else {
            return false;
        }
    }, p3, funD);

    Timepoint tp6 = CLOCK::now() + Millisecs(10);
    Millisecs p4(1);
    safePrint("FunE will be called with period " + to_string(p4.count()) + " ms until " + getTime(tp6));
    timer.registerTimer(tp6, p4, funE);
    
    
    // after 100 millisecond, exit the program
    this_thread::sleep_for(Millisecs(100));
    
    exit(EXIT_SUCCESS);    
}


