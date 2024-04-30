#ifndef RINGSWARM_TRANSPORTWRAPPER_H
#define RINGSWARM_TRANSPORTWRAPPER_H

#include "Transport.h"

namespace RingSwarm::transport {
class TransportWrapper : public Transport {
protected:
    std::unique_ptr<Transport> transport;

public:
    explicit TransportWrapper(std::unique_ptr<Transport> wrappedTransport) : transport(std::move(wrappedTransport)) {}

    async::Coroutine<> rawRead(void *data, uint32_t size) override {
        return transport->rawRead(data, size);
    }

    void rawWrite(void *data, uint32_t len) override {
        transport->rawWrite(data, len);
    }

    void close() override {
        transport->close();
    }

    ~TransportWrapper() override = default;
};
}// namespace RingSwarm::transport

#endif//RINGSWARM_TRANSPORTWRAPPER_H
