#include "ServerHandler.h"
#include "../core/RingSwarmException.h"
#include "../core/Settings.h"
#include "../transport/SecureOverlayTransport.h"
#include "RequestHeader.h"
#include "TooLargeMessageException.h"
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
    auto resp = std::make_shared<ResponseBuffer>(nodeListSize);
    resp->write(nodeList);
    transport->scheduleResponse(std::move(resp), responseType, tag);
}

void ServerHandler::listenRequest() {
    transport->rawRead(&nextHeader, sizeof(RequestHeader))->then([this]() {
        auto h = nextHeader;
        if (h.requestLength > MAX_REQUEST_LENGTH) {
            BOOST_LOG_TRIVIAL(trace) << "Got too long header: " << h.requestLength;
        }
        if (h.method == 0) {
            BOOST_LOG_TRIVIAL(trace) << "Got exit method";
        }
        if (h.method >= proto::ServerHandler::MethodsCount) {
            BOOST_LOG_TRIVIAL(trace) << "Got unknown command: " << (int) h.method;
        }
        if (transport->activeTags.test(h.tag)) {
            BOOST_LOG_TRIVIAL(trace) << "Got tags collision: " << (int) h.tag;
        }
        /*BOOST_LOG_TRIVIAL(trace) << "Got req header  |<=== " << ServerHandler::MethodNames[h.method] << " "
                                 << h.requestLength << " bytes. Tag: " << ((int) h.tag);*/
        transport->readBuffer(h.requestLength)->then([this, h](std::shared_ptr<transport::Buffer> b) {
            listenRequest();
            /*BOOST_LOG_TRIVIAL(trace) << "Got req body   |<=== " << ServerHandler::MethodNames[h.method] << " "
                                     << h.requestLength << " bytes. Tag: " << ((int) h.tag);*/
            (this->*ServerHandler::Methods[h.method])(*b.get(), h.tag);
        });
    });
}

void ServerHandler::Handle(transport::Transport *serverSide) {
    transport::SecureOverlayTransport::createServerSide(serverSide)->then([](auto *overlay) {
        auto *h = new ServerHandler(overlay);
        handlers.push_back(h);
        h->handleHandshake();
    });
}
}// namespace RingSwarm::proto