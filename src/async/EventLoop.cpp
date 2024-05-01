#include "EventLoop.h"

#include "../Assert.h"
#include "../core/Thread.h"

#include "EventLoopLock.h"

#include <boost/log/trivial.hpp>
#include <uvw/pipe.h>
#include <uvw/tcp.h>

#include <condition_variable>
#include <thread>

namespace RingSwarm::async {
using namespace uvw;

EventLoop *EventLoop::mainEventLoop = nullptr;

char smallBuffer[1024];

bool EventLoop::run() {
    Assert(uvwEventLoop == nullptr, "event loop re-entrance detected");
    uvwEventLoop = loop::create();
    uv_file fds[2];
    if (uv_pipe(fds, UV_NONBLOCK_PIPE, UV_NONBLOCK_PIPE) != 0) {
        uvwEventLoop = nullptr;
        return false;
    }
    reader = uvwEventLoop->resource<pipe_handle>();
    reader->open(fds[0]);
    reader->on<data_event>([](auto &event, auto &handler) {
        event.data.release();
    });
    reader->read<[](auto &p, auto suggested) {
        return std::pair<char *, decltype(suggested)>{
            smallBuffer,
            sizeof(smallBuffer)
        };
    }>();
    writer = uvwEventLoop->resource<pipe_handle>();
    writer->open(fds[1]);
    eventLoopThread = std::thread([this] {
        core::setThreadName("RingSwarm - libuv event loop");
        while (!stopping) {
            eventLoopEnteredLock.lock();
            loopRunResult = uvwEventLoop->run(uvw::loop::run_mode::ONCE);
            eventLoopEnteredLock.unlock();
            eventLoopExitedLock.lock();
            eventLoopExitedLock.unlock();
        }
    });
    auto ell = acquireEventLoopLock();
    return true;
}

int EventLoop::stop() {
    Assert(
        uvwEventLoop != nullptr && uvwEventLoop->alive(),
        "event loop must be alive"
    );
    stopping = true;
    {
        auto ell = acquireEventLoopLock();
        uvwEventLoop->stop();
    }
    eventLoopThread.join();
    return loopRunResult;
}

EventLoopLock EventLoop::acquireEventLoopLock() {
    Assert(
        uvwEventLoop != nullptr && uvwEventLoop->alive(),
        "event loop must be alive"
    );
    auto id = std::this_thread::get_id();
    if (currentExitedLockOwner == id || eventLoopThread.get_id() == id)
        return EventLoopLock(nullptr);
    eventLoopExitedLock.lock();
    EventLoopLock lg(this);
    currentExitedLockOwner = id;
    char c = 0;
    int rc;
    do { rc = writer->try_write(&c, 1); } while (rc == UV_EAGAIN);
    Assert(rc == 1, "incorrect bytes written count");
    eventLoopEnteredLock.lock();
    eventLoopEnteredLock.unlock();
    return std::move(lg);
}

std::shared_ptr<uvw::tcp_handle> EventLoop::createTcpSocket() {
    Assert(uvwEventLoop != nullptr, "event loop must be initialized");
    acquireEventLoopLock();
    return uvwEventLoop->resource<uvw::tcp_handle>();
}

std::shared_ptr<uvw::get_addr_info_req> EventLoop::createAddrInfoReq() {
    Assert(uvwEventLoop != nullptr, "event loop must be initialized");
    acquireEventLoopLock();
    return uvwEventLoop->resource<uvw::get_addr_info_req>();
}

void EventLoop::unlockEventLoop() {
    currentExitedLockOwner = std::thread::id();
    eventLoopExitedLock.unlock();
}
}// namespace RingSwarm::async
