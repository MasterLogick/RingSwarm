#ifndef RINGSWARM_BOOSTSOCKETTRANSPORT_H
#define RINGSWARM_BOOSTSOCKETTRANSPORT_H

#include <boost/asio/ip/tcp.hpp>
#include "Transport.h"

namespace RingSwarm::transport {

    class BoostSocketTransport : public Transport {
        boost::asio::ip::tcp::socket *sock;
    public:
        explicit BoostSocketTransport(boost::asio::ip::tcp::socket *sock);

        boost::asio::awaitable<void> close() override;

    private:
        boost::asio::awaitable<void> rawWrite(void *data, uint32_t len) override;

        boost::asio::awaitable<void> rawRead(void *buff, uint32_t len) override;
    };

}

#endif //RINGSWARM_BOOSTSOCKETTRANSPORT_H
