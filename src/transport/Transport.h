#ifndef RINGSWARM_TRANSPORT_H
#define RINGSWARM_TRANSPORT_H

#include <cstdint>
#include <experimental/coroutine>
#include <boost/asio/awaitable.hpp>
#include "Buffer.h"
#include "ResponseBuffer.h"
#include "RequestBuffer.h"
#include "../proto/ResponseHeader.h"

namespace RingSwarm::transport {
    class Transport {
        boost::asio::awaitable<uint32_t> readResponseLength(uint32_t maxResponseLength);

        boost::asio::awaitable<proto::ResponseHeader> readResponseHeader();

        virtual boost::asio::awaitable<void> rawWrite(void *data, uint32_t len) = 0;

        virtual boost::asio::awaitable<void> rawRead(void *buff, uint32_t len) = 0;

    public:

        boost::asio::awaitable<void> sendResponse(ResponseBuffer &resp);

        boost::asio::awaitable<void> startLongResponse(uint32_t size);

        boost::asio::awaitable<void> sendLongResponsePart(void *data, uint32_t len);

        boost::asio::awaitable<void> sendError();

        boost::asio::awaitable<void> sendEmptyResponse();

        boost::asio::awaitable<void> sendRequest(uint16_t commandIndex, RingSwarm::transport::RequestBuffer &req);

        boost::asio::awaitable<transport::Buffer> readResponse(uint32_t maxResponseLength);

        virtual boost::asio::awaitable<void> close() = 0;
    };
}

#endif //RINGSWARM_TRANSPORT_H
