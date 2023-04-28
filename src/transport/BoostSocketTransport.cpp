#include "BoostSocketTransport.h"

namespace RingSwarm::transport {
    boost::asio::awaitable<void> BoostSocketTransport::close() {
        sock->close();
        co_return;
    }

    boost::asio::awaitable<void> BoostSocketTransport::rawWrite(void *data, uint32_t len) {
        co_return;
    }

    boost::asio::awaitable<void> BoostSocketTransport::rawRead(void *buff, uint32_t len) {
        co_return;
    }

    BoostSocketTransport::BoostSocketTransport(boost::asio::ip::tcp::socket *sock)
            : sock(sock) {}
}