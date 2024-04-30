#include "ClientHandler.h"

namespace RingSwarm::proto {
ClientHandler::ClientHandler(std::unique_ptr<transport::Transport> transport, core::Node &remote)
    : clientSideWrapper(std::move(transport)), remote(remote) {}

core::Node &ClientHandler::getRemote() {
    return remote;
}
}// namespace RingSwarm::proto