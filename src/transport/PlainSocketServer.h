#ifndef RINGSWARM_PLAINSOCKETSERVER_H
#define RINGSWARM_PLAINSOCKETSERVER_H

#include "../async/coroutine.h"

#include <uvw/tcp.h>

#include <string>

namespace RingSwarm::transport {
class PlainSocketConnectionInfo;

class PlainSocketTransport;

class PlainSocketServer {
    std::shared_ptr<uvw::tcp_handle> serverHandler;
    std::function<void(std::shared_ptr<PlainSocketTransport>)> listenHandler;

public:
    PlainSocketServer(std::string host, int port);
    ~PlainSocketServer();

    int
    listen(std::function<void(std::shared_ptr<PlainSocketTransport>)> handler);
};
}// namespace RingSwarm::transport

#endif// RINGSWARM_PLAINSOCKETSERVER_H
