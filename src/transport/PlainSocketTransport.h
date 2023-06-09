#ifndef RINGSWARM_PLAINSOCKETTRANSPORT_H
#define RINGSWARM_PLAINSOCKETTRANSPORT_H

#include <string>
#include "Transport.h"
#include "../async/Future.h"
#include <uvw/tcp.h>
#include <queue>

namespace RingSwarm::transport {
    class PlainSocketTransport : public Transport {
        std::shared_ptr<uvw::tcp_handle> tcpHandler;
        transport::Buffer pending;
        transport::Buffer currentRequest;
        std::shared_ptr<async::Future<uint8_t *>> currentFuture;
        async::Spinlock accessSpinlock;
        std::queue<std::pair<std::shared_ptr<async::Future<uint8_t *>>, uint32_t >> readRequestQueue;
    public:
        PlainSocketTransport(std::string &host, int port);

        explicit PlainSocketTransport(const std::shared_ptr<uvw::tcp_handle> &handle);

        std::shared_ptr<async::Future<uint8_t *>> rawRead(uint32_t size) override;

        void rawWrite(void *data, uint32_t len) override;

        void close() override;

        ~PlainSocketTransport() override;

        void setupHandler();
    };
}

#endif //RINGSWARM_PLAINSOCKETTRANSPORT_H
