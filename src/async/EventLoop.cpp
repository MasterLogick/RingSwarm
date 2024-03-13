#include "EventLoop.h"

#include "../core/Thread.h"

#include <condition_variable>
#include <thread>

#include <boost/log/trivial.hpp>
#include <uvw/pipe.h>

namespace RingSwarm::async {
using namespace uvw;
std::shared_ptr<loop> eventLoop;

std::shared_ptr<loop> &getEventLoop() {
    return eventLoop;
}

std::shared_ptr<pipe_handle> reader;
std::shared_ptr<pipe_handle> writer;
std::mutex writeLock;
bool written = false;

bool initEventLoop() {
    eventLoop = loop::create();
    uv_file fds[2];
    if (uv_pipe(fds, UV_NONBLOCK_PIPE, UV_NONBLOCK_PIPE) != 0) {
        return false;
    }
    reader = eventLoop->resource<pipe_handle>();
    reader->open(fds[0]);
    reader->read();
    writer = eventLoop->resource<pipe_handle>();
    writer->open(fds[1]);
    std::thread([] {
        core::setThreadName("Event loop");
        async::getEventLoop()->run();
    }).detach();
    return true;
}

void interruptEventLoop() {
    std::lock_guard<std::mutex> l(writeLock);
    char c = 0;
    while (writer->try_write(&c, 1) == UV_EAGAIN) {}
}
}// namespace RingSwarm::async