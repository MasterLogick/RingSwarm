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
        std::shared_ptr<async::Future<void>> currentFuture;
        async::Spinlock accessSpinlock;
        std::queue<std::tuple<std::shared_ptr<async::Future<void>>, void *, uint32_t >> readRequestQueue;
        std::mutex writeLock;
/*        std::mutex waitLock;
        std::condition_variable writeCV;
        bool written = false;*/
    public:
        PlainSocketTransport(std::string &host, int port);

        explicit PlainSocketTransport(const std::shared_ptr<uvw::tcp_handle> &handle);

        std::shared_ptr<async::Future<void>> rawRead(void *data, uint32_t size) override;

        void rawWrite(void *data, uint32_t len) override;

        void close() override;

        ~PlainSocketTransport() override;

        void setupHandler();
    };
}

#endif //RINGSWARM_PLAINSOCKETTRANSPORT_H
