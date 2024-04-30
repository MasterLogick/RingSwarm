#ifndef RINGSWARM_PLAINSOCKETSERVER_H
#define RINGSWARM_PLAINSOCKETSERVER_H

#include "../async/coroutine.h"
#include <boost/asio/awaitable.hpp>
#include <string>
#include <uvw/tcp.h>

namespace RingSwarm::transport {
class PlainSocketConnectionInfo;

class PlainSocketTransport;

class PlainSocketServer {
    std::shared_ptr<uvw::tcp_handle> serverHandler;
    std::function<void(std::unique_ptr<PlainSocketTransport>)> listenHandler;

public:
    PlainSocketServer(std::string host, int port);
    ~PlainSocketServer();

    int listen(std::function<void(std::unique_ptr<PlainSocketTransport>)> handler);
};
}// namespace RingSwarm::transport

#endif//RINGSWARM_PLAINSOCKETSERVER_H
