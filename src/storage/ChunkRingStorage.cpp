#include "ChunkRingStorage.h"

#include "ClonedEntityException.h"
#include "KeyIndexedStorages.h"
#include "NodeStorage.h"
#include "Statement.h"
#include "StorageManager.h"

namespace RingSwarm::storage {
KeyIndexedStorage<core::ChunkRing *> chunkRingStorage;

const char *chunkSwarmNodeSelect = "select chunk_index, node_id\n"
                                   "from chunk_swarm\n"
                                   "where key_id = :key_id;";

core::ChunkRing *getChunkRing(core::Id *keyId) {
    if (chunkRingStorage.contains(keyId)) {
        return chunkRingStorage[keyId];
    }
    Statement chunkSwarmNodeSelectStatement(dbConnection, chunkSwarmNodeSelect);
    chunkSwarmNodeSelectStatement.bindId(":key_id", keyId);
    auto *ring = new core::ChunkRing();
    while (chunkSwarmNodeSelectStatement.nextRow()) {
        auto chunkIndex = chunkSwarmNodeSelectStatement.getInt64(0);
        auto *nodeId = chunkSwarmNodeSelectStatement.getId(1);
        auto *node = getNode(nodeId);
        if (node != nullptr) {
            /*if (!ring->contains(chunkIndex)) {
                (*ring)[chunkIndex] = std::vector<core::Node *>();
            }*/
            (*ring)[chunkIndex].push_back(node);
        } else {
            delete nodeId;
        }
    }
    chunkRingStorage[keyId] = ring;
    return ring;
}

const char *chunkSwarmNodeInsert =
    "insert into chunk_swarm (key_id, chunk_index, node_id)\n"
    "values (:key_id, :chunk_index, :node_id);";

void storeChunkRing(core::Id *keyId, core::ChunkRing *ring) {
    if (chunkRingStorage.contains(keyId)) {
        if (chunkRingStorage[keyId] != ring) {
            throw ClonedEntityException();
        }
        return;
    } else {
        chunkRingStorage[keyId] = ring;
    }

    Statement chunkSwarmNodeInsertStatement(dbConnection, chunkSwarmNodeInsert);
    chunkSwarmNodeInsertStatement.bindId(":key_id", keyId);
    for (const auto &pair: *ring) {
        chunkSwarmNodeInsertStatement.bindInt64(":chunk_index", pair.first);
        auto &swarm = pair.second;
        for (const auto &item: swarm) {
            chunkSwarmNodeInsertStatement.bindId(":node_id", item->id);
            chunkSwarmNodeInsertStatement.execute();
            chunkSwarmNodeInsertStatement.reset();
        }
    }
}
}// namespace RingSwarm::storage
