#ifndef RINGSWARM_TRANSPORTWRAPPER_H
#define RINGSWARM_TRANSPORTWRAPPER_H

#include "Transport.h"

namespace RingSwarm::transport {
    class TransportWrapper : public Transport {
    protected:
        Transport *transport;
    public:
        explicit TransportWrapper(Transport *transport) : transport(transport) {}

        std::shared_ptr<async::Future<uint8_t *>> rawRead(uint32_t size) override {
            return transport->rawRead(size);
        }

        void rawWrite(void *data, uint32_t len) override {
            transport->rawWrite(data, len);
        }

        void close() override {
            transport->close();
        }

        ~TransportWrapper() override {
            delete transport;
        }
    };
}

#endif //RINGSWARM_TRANSPORTWRAPPER_H
