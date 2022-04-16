#pragma once

#include "def.h"
#include "closure.h"
#include "flag.h"
#include "log.h"
#include "./co/sock.h"
#include "./co/event.h"
#include "./co/mutex.h"
#include "./co/pool.h"
#include "./co/chan.h"
#include "./co/io_event.h"
#include "./co/wait_group.h"
#include <vector>

namespace co {

/**
 * initialize the coroutine library
 *   - It will not call `flat::init()` or `log::init()`.
 */
__codec void init();

/**
 * This is equal to:
 *   ```cpp
 *   flag::init(argc, argv);
 *   log::init();
 *   co::init();
 *   ```
 */
__codec void init(int argc, char** argv);

/**
 * This is equal to:
 *   ```cpp
 *   flag::init(config);
 *   log::init();
 *   co::init();
 *   ```
 */
__codec void init(const char* config);

/**
 * The same as co::stop(), stop all schedulers.
 *   - If you are using co.dll on windows, it is better to call co::exit() manaully 
 *     at end of the main function, or the program may hang forever at exit.
 *   - It will also call `log::exit()` after the schedulers are stopped, if you have 
 *     called `co::init(argc, argv)` or `co::init(config)` before.
 */
__codec void exit();

/**
 * add a task, which will run as a coroutine 
 *   - It is thread-safe and can be called from anywhere. 
 *   - Closure created by new_closure() will delete itself after Closure::run() 
 *     is done. Users MUST NOT delete it manually.
 *   - Closure is an abstract base class, users are free to implement their own 
 *     subtype of Closure. This may be useful if users do not want a Closure to 
 *     delete itself. See details in co/closure.h.
 * 
 * @param cb  a pointer to a Closure created by new_closure(), or an user-defined Closure.
 */
__codec void go(Closure* cb);

/**
 * add a task, which will run as a coroutine 
 *   - eg.
 *     go(f);               // void f(); 
 *     go([]() { ... });    // lambda 
 *     go(std::bind(...));  // std::bind 
 * 
 *     std::function<void()> x(std::bind(...)); 
 *     go(x);               // std::function<void()> 
 *     go(&x);              // std::function<void()>* 
 *
 *   - If f is a pointer to std::function<void()>, users MUST ensure that the 
 *     object f points to is valid when Closure::run() is running. 
 * 
 * @param f  any runnable object, as long as we can call f() or (*f)().
 */
template<typename F>
inline void go(F&& f) {
    go(new_closure(std::forward<F>(f)));
}

/**
 * add a task, which will run as a coroutine 
 *   - eg.
 *     go(f, 8);   // void f(int);
 *     go(f, p);   // void f(void*);   void* p;
 *     go(f, o);   // void (T::*f)();  T* o;
 * 
 *     std::function<void(P)> x(std::bind(...));
 *     go(x, p);   // P p;
 *     go(&x, p);  // P p; 
 *
 *   - If f is a pointer to std::function<void(P)>, users MUST ensure that the 
 *     object f points to is valid when Closure::run() is running. 
 *
 * @param f  any runnable object, as long as we can call f(p), (*f)(p) or (p->*f)().
 * @param p  parameter of f, or a pointer to an object of class P if f is a method.
 */
template<typename F, typename P>
inline void go(F&& f, P&& p) {
    go(new_closure(std::forward<F>(f), std::forward<P>(p)));
}

/**
 * add a task, which will run as a coroutine 
 *   - eg.
 *     go(f, o, p);   // void (T::*f)(P);  T* o;  P p;
 
 * @param f  a pointer to a method with a parameter in class T.
 * @param t  a pointer to an object of class T.
 * @param p  parameter of f.
 */
template<typename F, typename T, typename P>
inline void go(F&& f, T* t, P&& p) {
    go(new_closure(std::forward<F>(f), t, std::forward<P>(p)));
}

/**
 * define main function
 *   - DEF_main can be used to ensure code in main function also runs in coroutine. 
 */
#define DEF_main(argc, argv) \
__codec DEC_bool(disable_co_exit); \
int _co_main(int argc, char** argv); \
int main(int argc, char** argv) { \
    co::init(argc, argv); \
    FLG_disable_co_exit = true; \
    int r; \
    co::WaitGroup wg; \
    wg.add(); \
    go([&](){ \
        r = _co_main(argc, argv); \
        wg.done(); \
    }); \
    wg.wait(); \
    FLG_disable_co_exit = false; \
    co::exit(); \
    return r; \
} \
int _co_main(int argc, char** argv)


class __codec Scheduler {
  public:
    void go(Closure* cb);

    template<typename F>
    inline void go(F&& f) {
        this->go(new_closure(std::forward<F>(f)));
    }

    template<typename F, typename P>
    inline void go(F&& f, P&& p) {
        this->go(new_closure(std::forward<F>(f), std::forward<P>(p)));
    }

    template<typename F, typename T, typename P>
    inline void go(F&& f, T* t, P&& p) {
        this->go(new_closure(std::forward<F>(f), t, std::forward<P>(p)));
    }

  protected:
    Scheduler() = default;
    ~Scheduler() = default;
};

/**
 * get all schedulers 
 *   
 * @return  a reference of an array, which stores pointers to all the Schedulers
 */
__codec const std::vector<Scheduler*>& all_schedulers();

/**
 * get the current scheduler
 * 
 * @return a pointer to the current scheduler, or NULL if called from a non-scheduler thread.
 */
__codec Scheduler* scheduler();

/**
 * get next scheduler 
 *   - It is useful when users want to create coroutines in the same scheduler. 
 *   - eg. 
 *     auto s = co::next_scheduler();
 *     s->go(f);     // void f();
 *     s->go(g, 7);  // void g(int);
 * 
 * @return a non-null pointer.
 */
__codec Scheduler* next_scheduler();

/**
 * get number of schedulers 
 *   - scheduler id is from 0 to scheduler_num() - 1. 
 *   - This function may be used to implement scheduler-local storage:  
 *                std::vector<T> xx(co::scheduler_num());  
 *     xx[co::scheduler_id()] can be used in a coroutine to access the storage for 
 *     the current scheduler thread.
 * 
 * @return  total number of the schedulers.
 */
__codec int scheduler_num();

/**
 * get id of the current scheduler 
 *   - It is EXPECTED to be called in a coroutine. 
 * 
 * @return  a non-negative id of the current scheduler, or -1 if the current thread 
 *          is not a scheduler thread.
 */
__codec int scheduler_id();

/**
 * get id of the current coroutine 
 *   - It is EXPECTED to be called in a coroutine. 
 *   - Each cocoutine has a unique id. 
 * 
 * @return  a non-negative id of the current coroutine, or -1 if the current thread 
 *          is not a scheduler thread.
 */
__codec int coroutine_id();

/**
 * add a timer for the current coroutine 
 *   - It MUST be called in a coroutine.
 *   - Users MUST call yield() to suspend the coroutine after a timer was added.
 *     When the timer expires, the scheduler will resume the coroutine.
 *
 * @param ms  timeout in milliseconds.
 */
__codec void add_timer(uint32 ms);

/**
 * add an IO event on a socket to the epoll 
 *   - It MUST be called in a coroutine.
 *   - Users MUST call yield() to suspend the coroutine after an event was added.
 *     When the event is present, the scheduler will resume the coroutine.
 *
 * @param fd  the socket.
 * @param ev  an IO event, either ev_read or ev_write.
 *
 * @return    true on success, false on error.
 */
__codec bool add_io_event(sock_t fd, io_event_t ev);

/**
 * delete an IO event from epoll
 *   - It MUST be called in a coroutine.
 */
__codec void del_io_event(sock_t fd, io_event_t ev);

/**
 * remove all events on the socket 
 *   - It MUST be called in a coroutine.
 */
__codec void del_io_event(sock_t fd);

/**
 * suspend the current coroutine 
 *   - It MUST be called in a coroutine. 
 *   - Usually, users should add an IO event, or a timer, or both in a coroutine, 
 *     and then call yield() to suspend the coroutine. When the event is present 
 *     or the timer expires, the scheduler will resume the coroutine. 
 */
__codec void yield();

/**
 * sleep for milliseconds 
 *   - It is EXPECTED to be called in a coroutine. 
 * 
 * @param ms  time in milliseconds
 */
__codec void sleep(uint32 ms);

/**
 * check whether the current coroutine has timed out 
 *   - It MUST be called in a coroutine.
 *   - When a coroutine returns from an API with a timeout like co::recv, users may 
 *     call co::timeout() to check whether the API call has timed out. 
 * 
 * @return  true if timed out, otherwise false.
 */
__codec bool timeout();

/**
 * check whether a pointer is on the stack of the current coroutine 
 *   - It MUST be called in a coroutine. 
 */
__codec bool on_stack(const void* p);

/**
 * stop all coroutine schedulers 
 *   - It is safe to call stop() from anywhere. 
 */
__codec void stop();

} // namespace co

using co::go;
