#include "EventLoopLock.h"

#include "EventLoop.h"

namespace RingSwarm::async {

EventLoopLock::~EventLoopLock() {
    if (el != nullptr)
        el->unlockEventLoop();
}
}// namespace RingSwarm::async
