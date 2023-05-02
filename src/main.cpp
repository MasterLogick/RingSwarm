#include <thread>
#include <iostream>
#include "transport/PlainSocketServer.h"
#include "transport/Transport.h"
#include "transport/PlainSocketTransport.h"
#include "proto/ClientHandler.h"
#include "storage/StorageManager.h"
#include "storage/FileSwarmStorage.h"
#include "storage/Statement.h"
#include "storage/ChunkRingStorage.h"

int main() {
    RingSwarm::core::Node::thisNode = new RingSwarm::core::Node();
    RingSwarm::storage::loadStorage("ring-swarm.sqlite3");
    RingSwarm::core::FileMeta m(
            RingSwarm::core::Id::fromHexRepresentation(
                    "0000000000000000000000000000000000000000000000000000000000000001"),
            1, 1, 1, 1, 1, {}, {},
            RingSwarm::core::Id::fromHexRepresentation(
                    "0000000000000000000000000000000000000000000000000000000000000001"));
    auto swarm = std::map<int, RingSwarm::core::Node *>();
    swarm[0] = RingSwarm::core::Node::thisNode;
    auto *metaSwarm = new RingSwarm::core::FileMetaSwarm(&m, swarm, RingSwarm::storage::getChunkRing(m.fileId));
    RingSwarm::storage::storeFileMetaSwarm(metaSwarm);
    RingSwarm::storage::getFileMetaSwarm(RingSwarm::core::Id::fromHexRepresentation(
            "0000000000000000000000000000000000000000000000000000000000000001"));
    atexit([] { RingSwarm::storage::closeStorage(); });
    std::string host("127.0.0.1");
    RingSwarm::transport::PlainSocketServer server(host, 12345, 8);
    std::thread serv([&server] { server.listen(); });
    RingSwarm::transport::PlainSocketTransport transport(host, 12345);
    RingSwarm::proto::ClientHandler client(&transport);
    RingSwarm::core::Id id{};
    client.unsubscribeOnChunkChange(&id, 0);
    std::cout << "1" << std::endl;
    std::cin.get();
}
