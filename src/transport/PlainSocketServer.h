#ifndef RINGSWARM_PLAINSOCKETSERVER_H
#define RINGSWARM_PLAINSOCKETSERVER_H

#include "connectionInfo/ConnectionInfo.h"
#include <boost/asio/awaitable.hpp>
#include <string>
#include <uvw/tcp.h>

namespace RingSwarm::transport {
class PlainSocketConnectionInfo;

class PlainSocketServer {
    std::shared_ptr<uvw::tcp_handle> serverHandler;
    PlainSocketConnectionInfo *connectionInfo;

public:
    PlainSocketServer(std::string &host, int port);

    void listen();

    RingSwarm::transport::ConnectionInfo *getConnectionInfo();
};
}// namespace RingSwarm::transport

#endif//RINGSWARM_PLAINSOCKETSERVER_H
