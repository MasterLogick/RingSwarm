#include "TransportServerSideWrapper.h"
#include "ResponseHeader.h"
#include <boost/log/trivial.hpp>

namespace RingSwarm::proto {
    void TransportServerSideWrapper::startLongResponse(uint32_t size, uint8_t responseType, uint8_t tag) {
        ResponseHeader d{};
        d.responseLen = size;
        d.responseType = responseType;
        d.tag = tag;
        responseSent = responseSent->then([&] {
            rawWrite(&d, sizeof(ResponseHeader));
            responseRemainderSize = size;
            pendingLongResponse = async::Future<void>::create();
            BOOST_LOG_TRIVIAL(trace) << "Started long response."
                                     << "Payload: " << size << " bytes. Response type: " << responseType << ". Tag: "
                                     << tag;
            return pendingLongResponse;
        });
    }

    void TransportServerSideWrapper::sendResponse(ResponseBuffer &resp, uint8_t responseType, uint8_t tag) {
        auto *header = reinterpret_cast<ResponseHeader *>(resp.data);
        header->responseLen = resp.len - sizeof(ResponseHeader);
        header->responseType = responseType;
        header->tag = tag;
        responseSent = responseSent->then([&] {
            //todo check if present
            activeTags.reset(tag);
            rawWrite(resp.data, resp.len);
            BOOST_LOG_TRIVIAL(trace) << "Sent response  |===> "
                                     << "Payload: " << resp.len << " bytes. Response type: "
                                     << responseType
                                     << ". Tag: " << tag;
        });
    }

    void TransportServerSideWrapper::rawWrite(void *data, uint32_t len) {
        TransportWrapper::rawWrite(data, len);
        if (responseRemainderSize > 0) {
            // todo assert(responseRemainderSize >= len)
            responseRemainderSize -= len;
            if (responseRemainderSize == 0) {
                pendingLongResponse->resolve();
            }
        }
    }
} // transport