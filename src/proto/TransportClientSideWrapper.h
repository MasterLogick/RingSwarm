#ifndef RINGSWARM_TRANSPORTCLIENTSIDEWRAPPER_H
#define RINGSWARM_TRANSPORTCLIENTSIDEWRAPPER_H

#include "../transport/TransportWrapper.h"
#include "ResponseHeader.h"
#include "RequestBuffer.h"

namespace RingSwarm::proto {
    class TransportClientSideWrapper : public transport::TransportWrapper {
        struct Request {
            uint32_t maxResponseSize;
            std::shared_ptr<async::Future<ResponseHeader>> future;
        };
        uint32_t responseRemainderSize = 0;
        std::atomic_uint8_t nextTag;
        async::Spinlock spinlock;
        std::map<uint8_t, Request> m;

        void runResponseHandlerIteration();

    public:

        explicit TransportClientSideWrapper(transport::Transport *transport) : transport::TransportWrapper(transport) {}

        std::shared_ptr<async::Future<ResponseHeader>> sendRequest(
                uint8_t commandIndex, RequestBuffer &req, uint32_t maxResponseSize);

        std::shared_ptr<async::Future<uint8_t *>> rawRead(uint32_t size) override;
    };
}

#endif //RINGSWARM_TRANSPORTCLIENTSIDEWRAPPER_H
