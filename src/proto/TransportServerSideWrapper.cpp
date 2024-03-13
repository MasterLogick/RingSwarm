#include "TransportServerSideWrapper.h"
#include "../transport/TransportBackendException.h"
#include "ResponseHeader.h"
#include <boost/log/trivial.hpp>

namespace RingSwarm::proto {
std::shared_ptr<async::Future<LongResponseTransport *>> TransportServerSideWrapper::scheduleLongResponse(
        uint32_t size,
        uint8_t responseType,
        uint8_t tag) {
    auto lrt = async::Future<LongResponseTransport *>::create();
    auto f = async::Future<void>::create();
    lock.lock();
    responseSent = responseSent->then([this, size, responseType, tag, f, lrt] {
        ResponseHeader d{};
        d.responseLen = size;
        d.responseType = responseType;
        d.tag = tag;
        TransportWrapper::rawWrite(&d, sizeof(ResponseHeader));
        if (size == 0) {
            f->resolve();
        }
        /*BOOST_LOG_TRIVIAL(trace) << "Started long response. "
                                 << "Payload: " << size << " bytes. Response type: " << ((int) responseType)
                                 << ". Tag: " << ((int) tag);*/
        lrt->resolve(new LongResponseTransport(transport, f, size));
        return f;
    });
    lock.unlock();
    return lrt;
}

void TransportServerSideWrapper::scheduleResponse(
        std::shared_ptr<ResponseBuffer> resp, uint8_t responseType, uint8_t tag) {
    lock.lock();
    responseSent = responseSent->then([this, tag, resp, responseType] {
        auto *header = reinterpret_cast<ResponseHeader *>(resp->data);
        header->responseLen = resp->len - sizeof(ResponseHeader);
        header->responseType = responseType;
        header->tag = tag;
        //todo check if present
        activeTags.reset(tag);
        TransportWrapper::rawWrite(resp->data, resp->len);
        /*BOOST_LOG_TRIVIAL(trace) << "Sent response  |===> "
                                 << "Payload: " << resp->len << " bytes. Response type: "
                                 << ((int) responseType) << ". Tag: " << ((int) tag);*/
    });
    lock.unlock();
}

void TransportServerSideWrapper::rawWrite(void *data, uint32_t len) {
    throw transport::TransportBackendException();
}
}// namespace RingSwarm::proto