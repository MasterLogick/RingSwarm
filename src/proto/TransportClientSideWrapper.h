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
        ResponseHeader nextHeader;
        uint32_t responseRemainderSize = 0;
        async::Spinlock spinlock;
        std::map<uint8_t, Request> m;

        void runResponseHandlerIteration();

    public:

        explicit TransportClientSideWrapper(transport::Transport *transport);

        std::shared_ptr<async::Future<ResponseHeader>> sendRequest(
                uint8_t commandIndex, RequestBuffer &req, uint32_t maxResponseSize);

        std::shared_ptr<async::Future<std::shared_ptr<transport::Buffer>>> sendShortRequest(
                uint8_t commandIndex, RequestBuffer &req, uint32_t maxResponseSize);

        std::shared_ptr<async::Future<void>> rawRead(void *data, uint32_t size) override;
    };
}

#endif //RINGSWARM_TRANSPORTCLIENTSIDEWRAPPER_H
