#include "ChunkRingStorage.h"
#include "Statement.h"
#include "StorageManager.h"

namespace RingSwarm::storage {

    core::ChunkRing *getChunkRing(core::Id *fileId) {}

    const char *chunkSwarmNodeInsert =
            "insert into chunk_swarm (file_id, chunk_index, node_id)\n"
            "values (:file_id, :chunk_index, :node_id);";

    void storeChunkRing(core::Id *fileId, core::ChunkRing *ring) {
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