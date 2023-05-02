#include "ChunkSwarmStorage.h"
#include "Statement.h"
#include "StorageManager.h"
#include "ChunkRingStorage.h"
#include "NodeStorage.h"

namespace RingSwarm::storage {
    const char *hostedChunkSelect =
            "select file_id, chunk_index, data_hash, sign\n"
            "from chunk_link\n"
            "where file_id = :file_id\n"
            "  and chunk_index = :chunk_index;";

    core::ChunkSwarm *getHostedChunkSwarm(core::Id *fileId, uint64_t index) {
        Statement hostedChunkSelectStatement(dbConnection, hostedChunkSelect);
        hostedChunkSelectStatement.bindId(":file_id", fileId);
        hostedChunkSelectStatement.bindInt64(":chunk_index", index);
        if (!hostedChunkSelectStatement.nextRow()) {
            return nullptr;
        }
        auto *link = new core::ChunkLink(
                hostedChunkSelectStatement.getId(0),
                hostedChunkSelectStatement.getInt64(1),
                hostedChunkSelectStatement.getId(2),
                hostedChunkSelectStatement.getBlob(3)
        );
        return new core::ChunkSwarm(link, getChunkRing(fileId));
    }

    const char *nearestChunkNodeSelect =
            "select node_id, chunk_index\n"
            "from chunk_swarm\n"
            "where file_id = :file_id\n"
            "order by abs(chunk_index - :chunk_index)\n"
            "limit 1;";

    core::Node *getNearestChunkNode(core::Id *fileId, uint64_t chunkIndex, uint8_t *retSwarmIndex) {
        Statement nearestChunkNodeSelectStatement(dbConnection, nearestChunkNodeSelect);
        nearestChunkNodeSelectStatement.bindId(":file_id", fileId);
        nearestChunkNodeSelectStatement.bindInt64(":chunk_index", chunkIndex);
        if (!nearestChunkNodeSelectStatement.nextRow()) {
            return nullptr;
        }
        auto *id = nearestChunkNodeSelectStatement.getId(0);
        *retSwarmIndex = nearestChunkNodeSelectStatement.getInt32(1);
        auto *node = getNode(id);
        if (node != nullptr) {
            return node;
        } else {
            delete id;
            return nullptr;
        }
    }

    const char *chunkLinkInsert =
            "insert into chunk_link (file_id, chunk_index, data_hash, sign)\n"
            "values (:file_id, :chunk_index, :data_hash, :sign);";

    void storeChunkSwarm(core::ChunkSwarm *chunkSwarm) {
        auto *link = chunkSwarm->link;
        Statement chunkLinkInsertStatement(dbConnection, chunkLinkInsert);
        chunkLinkInsertStatement.bindId(":file_id", link->file);
        chunkLinkInsertStatement.bindInt64(":chunk_index", link->chunkIndex);
        chunkLinkInsertStatement.bindId(":data_hash", link->dataHash);
        chunkLinkInsertStatement.bindBlob(":sign", link->sign);
        chunkLinkInsertStatement.execute();
        storeChunkRing(chunkSwarm->link->file, chunkSwarm->ring);
    }
}