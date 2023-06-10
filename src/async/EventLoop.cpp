#include <thread>
#include "EventLoop.h"
#include <uvw/tcp.h>
#include <boost/log/trivial.hpp>

namespace RingSwarm::async {
    using namespace uvw;
    std::shared_ptr<loop> eventLoop;

    std::shared_ptr<loop> &getEventLoop() {
        return eventLoop;
    }

    std::shared_ptr<tcp_handle> client1;
    std::shared_ptr<tcp_handle> client2;

    void setEventHandlers(std::shared_ptr<tcp_handle> handle) {
        handle->on<connect_event>([](auto &, auto &) {
            BOOST_LOG_TRIVIAL(trace) << "interrupter handle connected";
        });
        handle->on<write_event>([](auto &, auto &) {
//            BOOST_LOG_TRIVIAL(trace) << "interrupter handle sent data";
        });
        handle->on<data_event>([](auto &, auto &) {
//            BOOST_LOG_TRIVIAL(trace) << "interrupter handle got data";
        });
        handle->on<close_event>([](auto &, auto &) {
            BOOST_LOG_TRIVIAL(trace) << "interrupter handle is closed";
        });
        handle->on<error_event>([](auto &err, auto &s) {
            auto a = client1;
            auto b = client2;
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
            async::getEventLoop()->run();
        }).detach();
    }

    void interruptEventLoop() {
        char c = 0;
        client1->write(&c, 1);
//        BOOST_LOG_TRIVIAL(trace) << "event loop interrupted";
    }
}