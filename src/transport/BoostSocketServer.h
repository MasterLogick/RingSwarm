#ifndef RINGSWARM_BOOSTSOCKETSERVER_H
#define RINGSWARM_BOOSTSOCKETSERVER_H

#include <string>
#include <boost/asio/awaitable.hpp>

namespace RingSwarm::transport {
    class BoostSocketServer {
        int port;
    public:
        explicit BoostSocketServer(int port);

        boost::asio::awaitable<void> listen();
    };
}

#endif //RINGSWARM_BOOSTSOCKETSERVER_H
