#include "EventLoop.h"

#include "../core/Thread.h"

#include <condition_variable>
#include <thread>

#include <boost/log/trivial.hpp>
#include <uvw/tcp.h>

namespace RingSwarm::async {
using namespace uvw;
std::shared_ptr<loop> eventLoop;

std::shared_ptr<loop> &getEventLoop() {
    return eventLoop;
}

std::shared_ptr<tcp_handle> client1;
std::shared_ptr<tcp_handle> client2;
std::mutex writeLock;
bool written = false;

void setEventHandlers(std::shared_ptr<tcp_handle> handle) {
    handle->on<connect_event>([](auto &, auto &) {
        BOOST_LOG_TRIVIAL(trace) << "interrupter handle connected";
    });
    handle->on<close_event>([](auto &, auto &) {
        BOOST_LOG_TRIVIAL(trace) << "interrupter handle is closed";
    });
    handle->on<error_event>([](auto &err, auto &s) {
        auto a = client1;
        if (a.get() == &s) {
            BOOST_LOG_TRIVIAL(trace) << "interrupter handle 1 error: " << err.what();
        } else {
            BOOST_LOG_TRIVIAL(trace) << "interrupter handle 2 error: " << err.what();
        }
    });
}

void initEventLoop() {
    eventLoop = loop::create();
    client1 = eventLoop->resource<tcp_handle>();
    client2 = eventLoop->resource<tcp_handle>();
    auto server = eventLoop->resource<tcp_handle>();

    setEventHandlers(client1);
    setEventHandlers(client2);

    sockaddr_in6 addr{};
    addr.sin6_family = AF_INET;
    server->bind(reinterpret_cast<sockaddr &>(addr));
    auto port = server->sock().port;

    server->on<listen_event>([server](auto &, auto &) {
        server->accept(*client2);
        client2->read();
        server->close();
    });
    server->listen();
    client1->connect("127.0.0.1", port);

    client1->read();
    std::thread([] {
        core::setThreadName("Event loop");
        async::getEventLoop()->run();
    }).detach();
}

void interruptEventLoop() {
    std::lock_guard<std::mutex> l(writeLock);
    char c = 0;
    while (client1->try_write(&c, 1) == UV_EAGAIN)
        ;
}
}// namespace RingSwarm::async