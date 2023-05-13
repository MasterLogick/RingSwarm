#ifndef RINGSWARM_PLAINSOCKETSERVER_H
#define RINGSWARM_PLAINSOCKETSERVER_H

#include <string>
#include <boost/asio/awaitable.hpp>
#include "connectionInfo/ConnectionInfo.h"

namespace RingSwarm::transport {
    class PlainSocketConnectionInfo;

    class PlainSocketServer {
        int sockFd;
        PlainSocketConnectionInfo *connectionInfo;
    public:
        PlainSocketServer(std::string &hostname, int port, int backlog);

        void listen();

        RingSwarm::transport::ConnectionInfo *getConnectionInfo();
    };
}

#endif //RINGSWARM_PLAINSOCKETSERVER_H
