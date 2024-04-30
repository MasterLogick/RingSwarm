#ifndef RINGSWARM_EVENTLOOP_H
#define RINGSWARM_EVENTLOOP_H

#include "EventLoopLock.h"
#include <thread>
#include <uvw/dns.h>
#include <uvw/loop.h>
#include <uvw/tcp.h>

namespace RingSwarm::async {
class EventLoop {
    static EventLoop *mainEventLoop;

    std::shared_ptr<uvw::loop> uvwEventLoop;
    std::shared_ptr<uvw::pipe_handle> reader;
    std::shared_ptr<uvw::pipe_handle> writer;

    bool stopping;
    std::thread eventLoopThread;

    std::atomic<std::thread::id> currentExitedLockOwner;
    std::mutex eventLoopExitedLock;
    std::mutex eventLoopEnteredLock;

    int loopRunResult;

public:
    static EventLoop *getMainEventLoop() {
        return mainEventLoop;
    }
    static void setMainEventLoop(EventLoop *eventLoop) {
        mainEventLoop = eventLoop;
    }

    bool run();

    int stop();

    EventLoopLock acquireEventLoopLock();

    std::shared_ptr<uvw::tcp_handle> createTcpSocket();

    std::shared_ptr<uvw::get_addr_info_req> createAddrInfoReq();
    void unlockEventLoop();
};
}// namespace RingSwarm::async

#endif//RINGSWARM_EVENTLOOP_H
