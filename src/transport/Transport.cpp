#include "Transport.h"
#include "../proto/ResponseHeader.h"
#include "../proto/TooLargeMessageException.h"

namespace RingSwarm::transport {

    void Transport::sendResponse(ResponseBuffer &resp) {
        *reinterpret_cast<uint32_t *>(resp.getData()) = resp.getWrittenSize() - 4;
        rawWrite(resp.getData(), resp.getWrittenSize());
    }

    void Transport::startLongResponse(uint32_t size) {
        proto::ResponseHeader d{};
        d.responseLen = size;
        d.errorFlag = false;
        rawWrite(&d, 5);
    }

    void Transport::sendLongResponsePart(void *data, uint32_t len) {
        rawWrite(data, len);
    }

    void Transport::sendError() {
        proto::ResponseHeader d{};
        d.responseLen = 0;
        d.errorFlag = true;
        rawWrite(&d, 5);
    }

    void Transport::sendEmptyResponse() {
        startLongResponse(0);
    }

    void Transport::sendRequest(uint16_t commandIndex, RingSwarm::transport::RequestBuffer &req) {
        *reinterpret_cast<uint32_t *>(req.getData()) = req.getWrittenSize() - 6;
        *reinterpret_cast<uint16_t *>(req.getData() + 4) = commandIndex;
        rawWrite(req.getData(), req.getWrittenSize());
    }

    proto::ResponseHeader Transport::readResponseHeader() {
        proto::ResponseHeader h{};
        rawRead(&h, 5);
        return h;
    }

    uint32_t Transport::readResponseLength(uint32_t maxResponseLength) {
        auto respHeader = readResponseHeader();
        if (respHeader.responseLen > maxResponseLength) {
            throw proto::TooLargeMessageException();
        }
        if (respHeader.errorFlag) {
            throw proto::ProtocolException();
        }
        return respHeader.responseLen;
    }

    transport::Buffer Transport::readResponse(uint32_t maxResponseLength) {
        auto len = readResponseLength(maxResponseLength);
        transport::Buffer resp(len);
        rawRead(resp.getData(), len);
        return resp;
    }
}