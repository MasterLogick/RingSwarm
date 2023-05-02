#include "ChunkSwarmStorage.h"
#include "Statement.h"
#include "StorageManager.h"
#include "ChunkRingStorage.h"
#include "NodeStorage.h"
#include "StorageException.h"

namespace RingSwarm::storage {
    class ChunkIdComparator {
    public:
        bool operator()(const std::pair<core::Id *, uint64_t> &lhs, const std::pair<core::Id *, uint64_t> &rhs) const {
            if (*lhs.first == *rhs.first) {
                return lhs.second < rhs.second;
            } else {
                return core::Id::Comparator()(lhs.first, rhs.first);
            }
        }
    };

    std::map<std::pair<core::Id *, uint64_t>, core::ChunkSwarm *, ChunkIdComparator> chunkSwarmStorage;
    const char *hostedChunkSelect =
            "select file_id, chunk_index, data_hash, sign\n"
            "from chunk_link\n"
            "where file_id = :file_id\n"
            "  and chunk_index = :chunk_index;";

    core::ChunkSwarm *getHostedChunkSwarm(core::Id *fileId, uint64_t index) {
        if (chunkSwarmStorage.contains(std::pair(fileId, index))) {
            return chunkSwarmStorage[std::pair(fileId, index)];
        }
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
        auto *retVal = new core::ChunkSwarm(link, getChunkRing(fileId));
        chunkSwarmStorage[std::pair(fileId, index)] = retVal;
        return retVal;
    }

    const char *nearestChunkNodeSelect =
            "select node_id, chunk_index\n"
            "from chunk_swarm\n"
            "where file_id = :file_id\n"
            "order by abs(chunk_index - :chunk_index);";

    uint64_t dist(uint64_t a, uint64_t b) {
        return a > b ? a - b : b - a;
    }

    core::Node *getNearestChunkNode(core::Id *fileId, uint64_t chunkIndex, uint8_t *retSwarmIndex) {
        std::pair<std::pair<core::Id *, uint64_t>, core::ChunkSwarm *> min;
        min.first.first = nullptr;
        for (const auto &item: chunkSwarmStorage) {
            if (dist(item.first.second, chunkIndex) <= dist(min.first.second, chunkIndex)) {
                min = item;
            }
        }
        if (min.first.first != nullptr) {
            *retSwarmIndex = min.first.second;
            //todo randomize
            return (*min.second->ring)[min.first.second][0];
        }

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
        if (chunkSwarmStorage.contains(std::pair(chunkSwarm->link->file, chunkSwarm->link->chunkIndex))) {
            throw StorageException();
        } else {
            chunkSwarmStorage[std::pair(chunkSwarm->link->file, chunkSwarm->link->chunkIndex)] = chunkSwarm;
        }

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