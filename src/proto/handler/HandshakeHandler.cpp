#include "../ServerHandler.h"
#include "../ClientHandler.h"
#include <chrono>
#include <boost/log/trivial.hpp>
#include "../TooLargeMessageException.h"

namespace RingSwarm::proto {

    void ClientHandler::handshake() {
        uint32_t buffSize = core::Node::thisNode->getSerializedSize();
        transport::Buffer b(buffSize + 4);
        b.write<uint32_t>(buffSize);
        b.write(core::Node::thisNode);
        transport->rawWrite(b.getData(), b.getWrittenSize());
    }

    void ServerHandler::handleHandshake() {
        // todo add client verification
        uint32_t size;
        transport->rawRead(&size, 4);
        if (size > 1024 * 1024) {
            throw TooLargeMessageException();
        }

        transport::Buffer b(size);
        transport->rawRead(b.getData(), size);
        remote = b.read<core::Node *>();
        BOOST_LOG_TRIVIAL(debug) << "Got handshake from " << remote->id->getHexRepresentation();
    }
}