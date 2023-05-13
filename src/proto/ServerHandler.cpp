#include "ServerHandler.h"
#include "TooLargeMessageException.h"
#include "../core/RingSwarmException.h"
#include "RequestHeader.h"
#include "../transport/SecureOverlayTransport.h"
#include <boost/log/trivial.hpp>

#define MAX_REQUEST_LENGTH (1024 * 1024)
namespace RingSwarm::proto {
    void ServerHandler::handleClientConnection() {
//        try {
            handleHandshake();
            while (true) {
                RequestHeader requestHeader{};
                transport->rawRead(&requestHeader, 6);
                if (requestHeader.requestLength > MAX_REQUEST_LENGTH) {
                    throw TooLargeMessageException();
                }
                transport::Buffer buff(requestHeader.requestLength);
                transport->rawRead(buff.getData(), requestHeader.requestLength);
                if (requestHeader.method == 0) {
                    transport->close();
                    return;
                }
                if (requestHeader.method >= ServerHandler::MethodsCount) {
                    throw ProtocolException();
                }
                BOOST_LOG_TRIVIAL(trace) << "Enter " << ServerHandler::MethodNames[requestHeader.method]
                                         << " method handler";
                (this->*ServerHandler::Methods[requestHeader.method])(buff);
                BOOST_LOG_TRIVIAL(trace) << "Executed " << ServerHandler::MethodNames[requestHeader.method]
                                         << " method handler";
            }
        /*} catch (core::RingSwarmException &e) {
            BOOST_LOG_TRIVIAL(error) << e.what();
            errorStop();
            return;
        }*/
    }

    ServerHandler::ServerHandler(transport::Transport *transport) : transport(
            new transport::SecureOverlayTransport(transport)) {
    }

    void ServerHandler::errorStop() {
        transport->close();
    }

    void ServerHandler::sendNodeListResponse(std::vector<core::Node *> &nodeList) {
        uint32_t nodeListSize = 0;
        for (const auto &node: nodeList) {
            nodeListSize += node->getSerializedSize();
        }
        transport::ResponseBuffer resp(nodeListSize);
        for (const auto &node: nodeList) {
            resp.writeNode(node);
        }
        transport->sendResponse(resp);
    }
}