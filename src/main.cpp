#include <thread>
#include <iostream>
#include "transport/PlainSocketServer.h"
#include "transport/Transport.h"
#include "transport/PlainSocketTransport.h"
#include "proto/ClientHandler.h"
#include "storage/StorageManager.h"
#include "storage/FileSwarmStorage.h"
#include "storage/Statement.h"

int main() {
    RingSwarm::core::Node::thisNode = new RingSwarm::core::Node();
    RingSwarm::core::Node::thisNode->internalStorageId = 1;
    RingSwarm::storage::loadStorage("ring-swarm.sqlite3");
    RingSwarm::core::FileMeta m(
            0,
            RingSwarm::core::Id::fromHexRepresentation(
                    "0000000000000000000000000000000000000000000000000000000000000001"),
            1, 1, 1, 1, 1, {}, {},
            RingSwarm::core::Id::fromHexRepresentation(
                    "0000000000000000000000000000000000000000000000000000000000000001"));
    auto swarm = std::vector<RingSwarm::core::Node *>();
    swarm.push_back(RingSwarm::core::Node::thisNode);
    std::shared_ptr<RingSwarm::core::FileMetaSwarm> metaSwarm =
            std::make_shared<RingSwarm::core::FileMetaSwarm>(&m, 0, swarm);
    RingSwarm::storage::storeNewFileMetaSwarm(metaSwarm);
    RingSwarm::storage::getHostedFileMetaSwarm(RingSwarm::core::Id::fromHexRepresentation(
            "0000000000000000000000000000000000000000000000000000000000000001"));
    atexit([] { RingSwarm::storage::closeStorage(); });
    std::string host("127.0.0.1");
    RingSwarm::transport::PlainSocketServer server(host, 12345, 8);
    std::thread serv([&server] { server.listen(); });
    RingSwarm::transport::PlainSocketTransport transport(host, 12345);
    RingSwarm::proto::ClientHandler client(&transport);
    RingSwarm::core::Id id{};
    client.unsubscribeOnChunkChange(id, 0);
    std::cout << "1" << std::endl;
    std::cin.get();
}