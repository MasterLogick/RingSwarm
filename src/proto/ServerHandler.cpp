#include "ServerHandler.h"
#include "TooLargeMessageException.h"
#include "../core/RingSwarmException.h"
#include "RequestHeader.h"
#include "../transport/SecureOverlayTransport.h"
#include "../core/Settings.h"
#include <boost/log/trivial.hpp>

#define MAX_REQUEST_LENGTH (1024 * 1024)
namespace RingSwarm::proto {

    std::vector<ServerHandler *> handlers;

    ServerHandler::ServerHandler(transport::Transport *transport) : transport(
            new TransportServerSideWrapper(transport)) {
    }

    void ServerHandler::errorStop() {
        transport->close();
    }

    void ServerHandler::sendNodeListResponse(std::vector<core::Node *> &nodeList, uint8_t responseType, uint8_t tag) {
        uint32_t nodeListSize = 4;
        for (const auto &node: nodeList) {
            nodeListSize += node->getSerializedSize();
        }
        ResponseBuffer resp(nodeListSize);
        resp.write(nodeList);
        transport->sendResponse(resp, responseType, tag);
    }

    void ServerHandler::listenRequest() {
        transport->rawRead(sizeof(RequestHeader))->then([&](uint8_t *data) {
            auto h = *reinterpret_cast<RequestHeader *>(data);
            if (h.requestLength > MAX_REQUEST_LENGTH) {
                //todo handle bad request length
            }
            if (h.method == 0) {
                //todo handle exit
            }
            if (h.method >= proto::ServerHandler::MethodsCount) {
                //todo handle too large method
            }
            if (transport->activeTags.test(h.tag)) {
                //todo handle tags collision
            }
            transport->readBuffer(h.requestLength)->then([&](transport::Buffer b) {
                listenRequest();
                BOOST_LOG_TRIVIAL(trace) << "Got request    |<=== " << h.requestLength << " bytes";
                BOOST_LOG_TRIVIAL(trace) << "Executing " << ServerHandler::MethodNames[h.method]
                                         << " method handler";
                (this->*ServerHandler::Methods[h.method])(b, h.tag);
                BOOST_LOG_TRIVIAL(trace) << "Executed " << ServerHandler::MethodNames[h.method]
                                         << " method handler";
            });
        });
    }

    void ServerHandler::Handle(transport::Transport *serverSide) {
        transport::SecureOverlayTransport::createServerSide(serverSide)->then([](auto *overlay) {
            auto *h = new ServerHandler(overlay);
            handlers.push_back(h);
            h->listenRequest();
        });

    }
}