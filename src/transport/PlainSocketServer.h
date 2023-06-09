#ifndef RINGSWARM_PLAINSOCKETSERVER_H
#define RINGSWARM_PLAINSOCKETSERVER_H

#include <string>
#include <boost/asio/awaitable.hpp>
#include "connectionInfo/ConnectionInfo.h"
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
}

#endif //RINGSWARM_PLAINSOCKETSERVER_H
