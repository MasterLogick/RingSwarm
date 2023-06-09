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
        if (m.empty()) {
            auto f = (m[0] = {maxResponseSize, async::Future<ResponseHeader>::create()}).future;
            runResponseHandlerIteration();
            return f;
        } else {
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
            BOOST_LOG_TRIVIAL(trace) << "Sent request   |===> " << proto::ServerHandler::MethodNames[commandIndex]
                                     << " " << req.len << " bytes";
            return f;
        }
    }

    void TransportClientSideWrapper::runResponseHandlerIteration() {
        transport::TransportWrapper::rawRead(sizeof(ResponseHeader))->then([&](uint8_t *data) {
            ResponseHeader header = *reinterpret_cast<ResponseHeader *>(data);
            delete data;
            spinlock.lock();
            if (m.contains(header.tag)) {
                auto r = m[header.tag];
                m.erase(header.tag);
                spinlock.unlock();
                if (header.responseLen > r.maxResponseSize) {
                    //todo throw exception
                }
                responseRemainderSize = header.responseLen;
                if (header.responseLen == 0) {
                    runResponseHandlerIteration();
                }
                r.future->resolve(header);
            } else {
                spinlock.unlock();
                //todo do something in this case
            }
        });
    }

    std::shared_ptr<async::Future<uint8_t *>> TransportClientSideWrapper::rawRead(uint32_t size) {
        return TransportWrapper::rawRead(size)->template then<uint8_t *>([&](uint8_t *data) -> uint8_t * {
            // todo assert(size <= responseRemainderSize)
            responseRemainderSize -= size;
            if (responseRemainderSize == 0) {
                spinlock.lock();
                if (!m.empty()) {
                    runResponseHandlerIteration();
                }
                spinlock.unlock();
            }
            return data;
        });
    }
}