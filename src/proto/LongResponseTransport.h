#ifndef RINGSWARM_LONGRESPONSETRANSPORT_H
#define RINGSWARM_LONGRESPONSETRANSPORT_H

#include "../transport/Transport.h"
#include "../transport/TransportWrapper.h"

namespace RingSwarm::proto {
    class LongResponseTransport : public transport::TransportWrapper {
        std::shared_ptr<async::Future<void>> future;
        uint32_t responseSize;
    public:
        LongResponseTransport() = default;

        LongResponseTransport(
                transport::Transport *transport,
                std::shared_ptr<async::Future<void>> future,
                uint32_t responseSize
        );

        std::shared_ptr<async::Future<void>> rawRead(void *data, uint32_t size) override;

        void rawWrite(void *data, uint32_t len) override;

        void close() override;

        ~LongResponseTransport() override;
    };
}

#endif //RINGSWARM_LONGRESPONSETRANSPORT_H
