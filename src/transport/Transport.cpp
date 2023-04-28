#include "Transport.h"
#include "../proto/TooLargeMessageException.h"

namespace RingSwarm::transport {

    boost::asio::awaitable<void> Transport::sendResponse(ResponseBuffer &resp) {
        *reinterpret_cast<uint32_t *>(resp.getData()) = resp.getWrittenSize() - 4;
        co_await rawWrite(resp.getData(), resp.getWrittenSize());
    }

    boost::asio::awaitable<void> Transport::startLongResponse(uint32_t size) {
        proto::ResponseHeader d{};
        d.responseLen = size;
        d.errorFlag = false;
        co_await rawWrite(&d, 5);
    }

    boost::asio::awaitable<void> Transport::sendLongResponsePart(void *data, uint32_t len) {
        co_await rawWrite(data, len);
    }

    boost::asio::awaitable<void> Transport::sendError() {
        proto::ResponseHeader d{};
        d.responseLen = 0;
        d.errorFlag = true;
        co_await rawWrite(&d, 5);
    }

    boost::asio::awaitable<void> Transport::sendEmptyResponse() {
        co_await startLongResponse(0);
    }

    boost::asio::awaitable<void>
    Transport::sendRequest(uint16_t commandIndex, RingSwarm::transport::RequestBuffer &req) {
        *reinterpret_cast<uint32_t *>(req.getData()) = req.getWrittenSize() - 6;
        *reinterpret_cast<uint16_t *>(req.getData() + 4) = commandIndex;
        co_await rawWrite(req.getData(), req.getWrittenSize());
    }

    boost::asio::awaitable<proto::ResponseHeader> Transport::readResponseHeader() {
        proto::ResponseHeader h{};
        co_await rawRead(&h, 5);
        co_return h;
    }

    boost::asio::awaitable<uint32_t> Transport::readResponseLength(uint32_t maxResponseLength) {
        auto respHeader = co_await readResponseHeader();
        if (respHeader.responseLen > maxResponseLength) {
            throw proto::TooLargeMessageException();
        }
        if (respHeader.errorFlag) {
            throw proto::ProtocolException();
        }
        auto len = respHeader.responseLen;
        co_return len;
    }

    boost::asio::awaitable<transport::Buffer> Transport::readResponse(uint32_t maxResponseLength) {
        auto len = co_await readResponseLength(maxResponseLength);
        transport::Buffer resp(len);
        co_await rawRead(resp.getData(), len);
        co_return resp;
    }
}