#include "ChunkSwarmStorage.h"
#include "Statement.h"
#include "StorageManager.h"
#include "ChunkRingStorage.h"
#include "NodeStorage.h"
#include "ClonedEntityException.h"
#include "KeyIndexedStorages.h"

namespace RingSwarm::storage {


    ChunkIndexedStorage<core::ChunkSwarm *> chunkSwarmStorage;
    const char *hostedChunkSelect =
            "select key_id, chunk_index, data_hash, data_size, sign\n"
            "from chunk_link\n"
            "where key_id = :key_id\n"
            "  and chunk_index = :chunk_index;";

    core::ChunkSwarm *getHostedChunkSwarm(core::Id *keyId, uint64_t index) {
        if (chunkSwarmStorage.contains(std::pair(keyId, index))) {
            return chunkSwarmStorage[std::pair(keyId, index)];
        }
        Statement hostedChunkSelectStatement(dbConnection, hostedChunkSelect);
        hostedChunkSelectStatement.bindId(":key_id", keyId);
        hostedChunkSelectStatement.bindInt64(":chunk_index", index);
        if (!hostedChunkSelectStatement.nextRow()) {
            return nullptr;
        }
        auto sign = new crypto::Signature();
        auto blobSign = hostedChunkSelectStatement.getBlob(4);
        std::copy(blobSign.begin(), blobSign.end(), sign->begin());
        auto *link = new core::ChunkLink(
                hostedChunkSelectStatement.getId(0),
                hostedChunkSelectStatement.getInt64(1),
                hostedChunkSelectStatement.getId(2),
                hostedChunkSelectStatement.getInt64(3),
                sign
        );
        auto *retVal = new core::ChunkSwarm(link, getChunkRing(keyId));
        chunkSwarmStorage[std::pair(keyId, index)] = retVal;
        return retVal;
    }

    // todo fix distance on ring function
    const char *nearestChunkNodeSelect =
            "select node_id, chunk_index\n"
            "from chunk_swarm\n"
            "where key_id = :key_id\n"
            "order by abs(chunk_index - :chunk_index);";

    uint64_t dist(uint64_t a, uint64_t b) {
        return a > b ? a - b : b - a;
    }

    core::Node *getNearestChunkNode(core::Id *keyId, uint64_t chunkIndex, uint8_t *retSwarmIndex) {
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
        nearestChunkNodeSelectStatement.bindId(":key_id", keyId);
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
            "insert into chunk_link (key_id, chunk_index, data_hash, data_size, sign)\n"
            "values (:key_id, :chunk_index, :data_hash, :data_size, :sign);";

    void storeChunkSwarm(core::ChunkSwarm *chunkSwarm) {
        if (chunkSwarmStorage.contains(std::pair(chunkSwarm->link->keyId, chunkSwarm->link->chunkIndex))) {
            if (chunkSwarmStorage[std::pair(chunkSwarm->link->keyId, chunkSwarm->link->chunkIndex)] != chunkSwarm) {
                throw ClonedEntityException();
            }
            return;
        } else {
            chunkSwarmStorage[std::pair(chunkSwarm->link->keyId, chunkSwarm->link->chunkIndex)] = chunkSwarm;
        }

        auto *link = chunkSwarm->link;
        Statement chunkLinkInsertStatement(dbConnection, chunkLinkInsert);
        chunkLinkInsertStatement.bindId(":key_id", link->keyId);
        chunkLinkInsertStatement.bindInt64(":chunk_index", link->chunkIndex);
        chunkLinkInsertStatement.bindId(":data_hash", link->dataHash);
        chunkLinkInsertStatement.bindInt64(":data_size", link->dataSize);
        chunkLinkInsertStatement.bindSignature(":sign", link->sign);
        chunkLinkInsertStatement.execute();
        storeChunkRing(chunkSwarm->link->keyId, chunkSwarm->ring);
    }
}