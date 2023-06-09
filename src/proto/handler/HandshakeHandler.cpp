#include "../ServerHandler.h"
#include "../ClientHandler.h"
#include <boost/log/trivial.hpp>
#include "../TooLargeMessageException.h"

namespace RingSwarm::proto {

    void ClientHandler::handshake() {
        uint32_t buffSize = core::Node::thisNode->getSerializedSize();
        transport::Buffer b(buffSize + 4);
        b.write<uint32_t>(buffSize);
        b.write(core::Node::thisNode);
        transport->rawWrite(b.data, b.len);
    }

    void ServerHandler::handleHandshake() {
        // todo add client verification
        transport->rawRead(4)->then([&](uint8_t *d) {
            auto *size = reinterpret_cast<uint32_t *>(d);
            if (*size > 1024 * 1024) {
                //todo throw TooLargeMessageException
            }
            transport->readBuffer(*size)->then([&](transport::Buffer b) {
                remote = b.read<core::Node *>();
                BOOST_LOG_TRIVIAL(debug) << "Got handshake from " << remote->id->getHexRepresentation();
                listenRequest();
            });
        });
    }
}