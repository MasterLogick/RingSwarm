#ifndef RINGSWARM_TRANSPORT_H
#define RINGSWARM_TRANSPORT_H

#include <cstdint>
#include "Buffer.h"
#include "ResponseBuffer.h"
#include "RequestBuffer.h"
#include "../proto/ResponseHeader.h"

namespace RingSwarm::transport {
    class Transport {
    public:
        void sendResponse(ResponseBuffer &resp);

        void startLongResponse(uint32_t size);

        void sendLongResponsePart(void *data, uint32_t len);

        virtual void rawWrite(void *data, uint32_t len) = 0;

        virtual void rawRead(void *buff, uint32_t len) = 0;

        virtual void close() = 0;

        void sendError();

        void sendEmptyResponse();

        void sendRequest(uint16_t commandIndex, RingSwarm::transport::RequestBuffer &req);

        proto::ResponseHeader readResponseHeader();

        uint32_t readResponseLength(uint32_t maxResponseLength);

        transport::Buffer readResponse(uint32_t maxResponseLength);
    };
}

#endif //RINGSWARM_TRANSPORT_H
