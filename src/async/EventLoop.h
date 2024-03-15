#ifndef RINGSWARM_EVENTLOOP_H
#define RINGSWARM_EVENTLOOP_H

#include <uvw/loop.h>

namespace RingSwarm::async {
std::shared_ptr<uvw::loop> &getEventLoop();

bool initEventLoop();

void withEventLoopLock(std::function<void(void)> f);
}// namespace RingSwarm::async

#endif//RINGSWARM_EVENTLOOP_H
