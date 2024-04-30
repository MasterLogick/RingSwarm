#include "PlainSocketServer.h"
#include "../Assert.h"
#include "../async/EventLoop.h"
#include "../proto/ServerHandler.h"
#include "PlainSocketTransport.h"
#include <boost/log/trivial.hpp>
#include <thread>

namespace RingSwarm::transport {

PlainSocketServer::PlainSocketServer(std::string host, int port) {
    auto ell = async::EventLoop::getMainEventLoop()->acquireEventLoopLock();
    serverHandler = async::EventLoop::getMainEventLoop()->createTcpSocket();
    serverHandler->on<uvw::error_event>([](auto &event, auto &) {
        BOOST_LOG_TRIVIAL(error) << "Plain tcp server error: " << event.what();
    });

    int err = serverHandler->bind(host, port);
    if (err != 0) {
        BOOST_LOG_TRIVIAL(trace) << "Plain tcp server could not bind on " << host << ":" << port << ". err: " << err;
        //todo handle
    }
}

int PlainSocketServer::listen(std::function<void(std::unique_ptr<PlainSocketTransport>)> handler) {
    Assert(listenHandler == nullptr, "server listen handler must be null before listening");
    listenHandler = std::move(handler);
    auto ell = async::EventLoop::getMainEventLoop()->acquireEventLoopLock();
    BOOST_LOG_TRIVIAL(debug) << "Plain tcp server starts listening";
    serverHandler->on<uvw::listen_event>([this](auto &event, uvw::tcp_handle &server) {
        auto socket = async::EventLoop::getMainEventLoop()->createTcpSocket();
        server.accept(*socket);
        auto peer = socket->peer();
        auto sock = socket->sock();
        BOOST_LOG_TRIVIAL(debug) << "Plain tcp server accepted connection from " << peer.ip << ":" << peer.port << " to " << sock.ip << ":" << sock.port;
        listenHandler(std::make_unique<PlainSocketTransport>(socket));
    });
    int err = serverHandler->listen();
    if (err != 0) {
        BOOST_LOG_TRIVIAL(error) << "Plain tcp server error: " << err;
    }
    return err;
}
PlainSocketServer::~PlainSocketServer() {
    auto ell = async::EventLoop::getMainEventLoop()->acquireEventLoopLock();
    serverHandler->close();
}
}// namespace RingSwarm::transport