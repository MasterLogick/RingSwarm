#include "ChunkRingStorage.h"
#include "Statement.h"
#include "StorageManager.h"
#include "NodeStorage.h"
#include "StorageException.h"

namespace RingSwarm::storage {
    std::map<core::Id *, core::ChunkRing *, core::Id::Comparator> chunkRingStorage;

    const char *chunkSwarmNodeSelect =
            "select chunk_index, node_id\n"
            "from chunk_swarm\n"
            "where file_id = :file_id;";

    core::ChunkRing *getChunkRing(core::Id *fileId) {
        if (chunkRingStorage.contains(fileId)) {
            return chunkRingStorage[fileId];
        }
        Statement chunkSwarmNodeSelectStatement(dbConnection, chunkSwarmNodeSelect);
        chunkSwarmNodeSelectStatement.bindId(":dile_id", fileId);
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
        chunkRingStorage[fileId] = ring;
        return ring;
    }

    const char *chunkSwarmNodeInsert =
            "insert into chunk_swarm (file_id, chunk_index, node_id)\n"
            "values (:file_id, :chunk_index, :node_id);";

    void storeChunkRing(core::Id *fileId, core::ChunkRing *ring) {
        if (chunkRingStorage.contains(fileId)) {
            throw StorageException();
        } else {
            chunkRingStorage[fileId] = ring;
        }

        Statement chunkSwarmNodeInsertStatement(dbConnection, chunkSwarmNodeInsert);
        chunkSwarmNodeInsertStatement.bindId(":file_id", fileId);
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
}