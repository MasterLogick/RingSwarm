#include "KeySwarmStorage.h"
#include "ChunkRingStorage.h"
#include "ClonedEntityException.h"
#include "StorageManager.h"
#include "Statement.h"
#include "NodeStorage.h"

namespace RingSwarm::storage {
    std::map<core::Id *, core::KeySwarm *, core::Id::Comparator> keySwarmStorage;

    const char *keySelect =
            "select public_key\n"
            "from keys\n"
            "where key_id = :key_id;";

    const char *keySwarmNodeSelect =
            "select node_id, swarm_index\n"
            "from key_swarm\n"
            "where key_id = :key_id;";

    core::KeySwarm *getKeySwarm(core::Id *keyId) {
        if (keySwarmStorage.contains(keyId)) {
            return keySwarmStorage[keyId];
        }
        Statement keySelectStatement(dbConnection, keySelect);
        keySelectStatement.bindId(":key_id", keyId);
        if (keySelectStatement.nextRow()) {
            auto *ks = new core::KeySwarm(keyId, keySelectStatement.getPublicKey(0), {}, getChunkRing(keyId));
            Statement keySwarmNodeSelectStatement(dbConnection, keySwarmNodeSelect);
            keySwarmNodeSelectStatement.bindId(":key_id", keyId);
            while (keySwarmNodeSelectStatement.nextRow()) {
                auto *nodeId = keySwarmNodeSelectStatement.getId(0);
                auto *node = storage::getNode(nodeId);
                if (node == nullptr) {
                    delete nodeId;
                    continue;
                }
                ks->swarm[keySwarmNodeSelectStatement.getInt32(1)] = node;
            }
            return ks;
        }
        return nullptr;
    }

    const char *keyInsert =
            "insert into keys (key_id, public_key)\n"
            "values (:key_id, :public_key);";
    const char *keySwarmNodeInsert =
            "insert into key_swarm (key_id, swarm_index, node_id)\n"
            "values (:key_id, :swarm_index, :node_id)";

    void storeKeySwarm(core::KeySwarm *keySwarm) {
        if (keySwarmStorage.contains(keySwarm->keyId)) {
            if (keySwarmStorage[keySwarm->keyId] != keySwarm) {
                throw ClonedEntityException();
            }
            return;
        }
        keySwarmStorage[keySwarm->keyId] = keySwarm;

        Statement keyInsertStatement(dbConnection, keyInsert);
        keyInsertStatement.bindId(":key_id", keySwarm->keyId);
        keyInsertStatement.bindPublicKey(":public_key", keySwarm->key);
        keyInsertStatement.execute();

        Statement keySwarmNodeInsertStatement(dbConnection, keySwarmNodeInsert);
        keySwarmNodeInsertStatement.bindId(":key_id", keySwarm->keyId);
        for (const auto &item: keySwarm->swarm) {
            keySwarmNodeInsertStatement.bindInt32(":swarm_index", item.first);
            keySwarmNodeInsertStatement.bindId(":node_id", item.second->id);
            keySwarmNodeInsertStatement.execute();
            keySwarmNodeInsertStatement.reset();
            storage::storeNode(item.second);
        }

        storeChunkRing(keySwarm->keyId, keySwarm->ring);
    }
}