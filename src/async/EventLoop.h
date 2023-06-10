#ifndef RINGSWARM_EVENTLOOP_H
#define RINGSWARM_EVENTLOOP_H

#include <uvw/loop.h>

namespace RingSwarm::async {
    std::shared_ptr<uvw::loop> &getEventLoop();

    void initEventLoop();

    void interruptEventLoop();
}

#endif //RINGSWARM_EVENTLOOP_H
