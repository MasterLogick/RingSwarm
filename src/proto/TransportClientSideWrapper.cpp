#include "TransportClientSideWrapper.h"
#include "RequestHeader.h"
#include "ServerHandler.h"
#include <boost/log/trivial.hpp>

namespace RingSwarm::proto {

std::shared_ptr<async::Future<ResponseHeader>> TransportClientSideWrapper::sendRequest(
        uint8_t commandIndex,
        RequestBuffer &req,
        uint32_t maxResponseSize) {
    spinlock.lock();
    if (m.size() == 256) {
        //todo wait until one tag is free free
    }
    uint8_t tag = 0;
    std::shared_ptr<async::Future<ResponseHeader>> f;
    for (int i = 0; i < 256; ++i) {
        auto p = m.try_emplace(i, maxResponseSize,
                               async::Future<ResponseHeader>::create());
        if (p.second) {
            tag = i;
            f = p.first->second.future;
            break;
        }
    }
    spinlock.unlock();

    req.assertFullyUsed();
    auto *header = reinterpret_cast<RequestHeader *>(req.data);
    header->requestLength = req.len - sizeof(RequestHeader);
    header->method = commandIndex;
    header->tag = tag;
    rawWrite(req.data, req.len);
    /*BOOST_LOG_TRIVIAL(trace) << "Sent request   |===> " << proto::ServerHandler::MethodNames[commandIndex]
                             << " " << req.len - sizeof(RequestHeader) << " bytes. Tag: " << ((int) tag);*/
    return f;
}

void TransportClientSideWrapper::runResponseHandlerIteration() {
    transport::TransportWrapper::rawRead(&nextHeader, sizeof(ResponseHeader))->then([this]() {
        if (responseRemainderSize != 0) {
            BOOST_LOG_TRIVIAL(trace) << "undefined behaviour";
        }
        ResponseHeader header = nextHeader;
        spinlock.lock();
        if (m.contains(header.tag)) {
            auto r = m[header.tag];
            m.erase(header.tag);
            if (header.responseLen > r.maxResponseSize) {
                /*BOOST_LOG_TRIVIAL(trace) << "Got too long response: " << header.responseLen << " bytes. Max: "
                                         << r.maxResponseSize << " bytes";*/
            }
            responseRemainderSize = header.responseLen;
            spinlock.unlock();
            if (header.responseLen == 0) {
                runResponseHandlerIteration();
            }
            /*BOOST_LOG_TRIVIAL(trace) << "Got resp header|<=== Payload: " << header.responseLen << " bytes. Type: "
                                     << ((int) header.responseType) << " Tag: " << ((int) header.tag);*/
            r.future->resolve(header);
        } else {
            spinlock.unlock();
            //todo do something in this case
        }
    });
}

std::shared_ptr<async::Future<void>> TransportClientSideWrapper::rawRead(void *data, uint32_t size) {
    return TransportWrapper::rawRead(data, size)->then([this, size]() {
        // todo assert(size <= responseRemainderSize)
        responseRemainderSize -= size;
        if (responseRemainderSize == 0) {
            //                BOOST_LOG_TRIVIAL(trace) << "read response buffer";
            spinlock.lock();
            runResponseHandlerIteration();
            spinlock.unlock();
        }
    });
}

std::shared_ptr<async::Future<std::shared_ptr<transport::Buffer>>>
TransportClientSideWrapper::sendShortRequest(uint8_t commandIndex, RequestBuffer &req, uint32_t maxResponseSize) {
    return sendRequest(commandIndex, req, maxResponseSize)->then<std::shared_ptr<transport::Buffer>>([this](ResponseHeader h) {
        auto *buff = new uint8_t[h.responseLen];
        return rawRead(buff, h.responseLen)->then<std::shared_ptr<transport::Buffer>>([buff, len = h.responseLen]() {
            return std::make_shared<transport::Buffer>(buff, len);
        });
    });
}

TransportClientSideWrapper::TransportClientSideWrapper(transport::Transport *transport)
    : transport::TransportWrapper(transport) {
    runResponseHandlerIteration();
}
}// namespace RingSwarm::proto