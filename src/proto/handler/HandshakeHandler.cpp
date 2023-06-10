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
        auto *headerSize = new uint32_t();
        transport->rawRead(headerSize, 4)->then([this, headerSize]() {
            if (*headerSize > 1024 * 1024) {
                //todo throw TooLargeMessageException
            }
            transport->readBuffer(*headerSize)->then([this](auto b) {
                remote = b->template read<core::Node *>();
                BOOST_LOG_TRIVIAL(debug) << "Got handshake from " << remote->id->getHexRepresentation();
                listenRequest();
            });
        });
    }
}