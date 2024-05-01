#ifndef RINGSWARM_SRC_ASYNC_EVENTLOOPLOCK_H
#define RINGSWARM_SRC_ASYNC_EVENTLOOPLOCK_H

#include <mutex>

namespace RingSwarm::async {
class EventLoop;

class EventLoopLock {
    EventLoop *el;

public:
    explicit EventLoopLock(EventLoop *el) : el(el) {}

    EventLoopLock(const EventLoopLock &) = delete;

    EventLoopLock(EventLoopLock &&lock) noexcept {
        el = lock.el;
        lock.el = nullptr;
    }

    ~EventLoopLock();
};
}// namespace RingSwarm::async

#endif// RINGSWARM_SRC_ASYNC_EVENTLOOPLOCK_H
