#ifndef RINGSWARM_PLAINSOCKETSERVER_H
#define RINGSWARM_PLAINSOCKETSERVER_H

#include <string>
#include <boost/asio/awaitable.hpp>

namespace RingSwarm::transport {
    class PlainSocketServer {
        int sockFd;
    public:
        explicit PlainSocketServer(std::string &hostname, int port, int backlog);

        void listen();
    };
}

#endif //RINGSWARM_PLAINSOCKETSERVER_H
