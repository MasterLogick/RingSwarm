#include "EventLoop.h"

namespace RingSwarm::async {
    std::shared_ptr<uvw::loop> eventLoop;

    std::shared_ptr<uvw::loop> &getEventLoop() {
        return eventLoop;
    }

    void initEventLoop() {
        eventLoop = uvw::loop::create();
//        eventLoop->configure(uvw::details::uvw_loop_option::IDLE_TIME,)
    }
}