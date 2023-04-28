#include "../ServerHandler.h"
#include "../ClientHandler.h"

namespace RingSwarm::proto {

    boost::asio::awaitable<void> ClientHandler::handshake() {
        co_return;
    }

    void ServerHandler::handleHandshake() {

    }
}