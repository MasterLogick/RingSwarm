#include "Transport.h"
#include "../proto/TooLargeMessageException.h"
#include "../proto/ServerHandler.h"
#include <boost/log/trivial.hpp>

namespace RingSwarm::transport {

    void Transport::sendResponse(ResponseBuffer &resp) {
        *reinterpret_cast<uint32_t *>(resp.getData()) = resp.getWrittenSize() - 5;
        resp.getData()[4] = 0;
        rawWrite(resp.getData(), resp.getWrittenSize());
        BOOST_LOG_TRIVIAL(trace) << "Sent response  |===> " << resp.getWrittenSize() << " bytes";
    }

    void Transport::startLongResponse(uint32_t size) {
        proto::ResponseHeader d{};
        d.responseLen = size;
        d.errorFlag = false;
        rawWrite(&d, 5);
        BOOST_LOG_TRIVIAL(trace) << "Started long response (" << size << " bytes of data)";
    }

    void Transport::sendLongResponsePart(void *data, uint32_t len) {
        rawWrite(data, len);
        BOOST_LOG_TRIVIAL(trace) << "Sent long response part (" << len << " bytes of data)";
    }

    void Transport::sendError() {
        proto::ResponseHeader d{};
        d.responseLen = 0;
        d.errorFlag = true;
        rawWrite(&d, 5);
        BOOST_LOG_TRIVIAL(trace) << "Sent error";
    }

    void Transport::sendEmptyResponse() {
        startLongResponse(0);
        BOOST_LOG_TRIVIAL(trace) << "Sent empty response";
    }

    void Transport::sendRequest(uint16_t commandIndex, RingSwarm::transport::RequestBuffer &req) {
        *reinterpret_cast<uint32_t *>(req.getData()) = req.getWrittenSize() - 6;
        *reinterpret_cast<uint16_t *>(req.getData() + 4) = commandIndex;
        req.assertFullyUsed();
        rawWrite(req.getData(), req.getWrittenSize());
        BOOST_LOG_TRIVIAL(trace) << "Sent request   |===> " << proto::ServerHandler::MethodNames[commandIndex]
                                 << " " << req.getWrittenSize() << " bytes";
    }

    proto::ResponseHeader Transport::readResponseHeader() {
        proto::ResponseHeader h{};
        rawRead(&h, 5);
        return h;
    }

    uint32_t Transport::readResponseLength(uint32_t maxResponseLength) {
        auto respHeader = readResponseHeader();
        if (respHeader.responseLen > maxResponseLength) {
            BOOST_LOG_TRIVIAL(trace) << "Got too big response of " << respHeader.responseLen << " bytes. Max: "
                                     << maxResponseLength << " bytes";
            throw proto::TooLargeMessageException();
        }
        if (respHeader.errorFlag) {
            BOOST_LOG_TRIVIAL(trace) << "Got error";
            throw proto::ProtocolException();
        }
        return respHeader.responseLen;
    }

    transport::Buffer Transport::readResponse(uint32_t maxResponseLength) {
        auto len = readResponseLength(maxResponseLength);
        transport::Buffer resp(len);
        rawRead(resp.getData(), len);
        BOOST_LOG_TRIVIAL(trace) << "Got response   |<=== " << len << " bytes";
        return resp;
    }
}