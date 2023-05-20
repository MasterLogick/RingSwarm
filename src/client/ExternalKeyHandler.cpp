#include "ExternalKeyHandler.h"
#include "../core/ConnectionManager.h"

namespace RingSwarm::client {
    ExternalKeyHandler::ExternalKeyHandler(core::Id *keyId, core::PublicKey *key, proto::ClientHandler *possibleKeySwarmNode) :
            keyId(keyId), key(key) {
        auto keySwarm = possibleKeySwarmNode->getKeySwarm(keyId);
        keySwarmNode = core::getOrConnectToOne(keySwarm);
        if (keySwarmNode == nullptr) {
            //todo throw and search for next entry point
        }
        auto zeroChunkSwarm = keySwarmNode->getChunkSwarm(keyId, 0);
        zeroChunkNode = core::getOrConnectToOne(zeroChunkSwarm);

        auto readSize = zeroChunkNode->getChunk(keyId, 0, 0, &keyInfo, sizeof(core::KeyInfo));
        if (readSize != sizeof(keyInfo)) {
            //todo throw broken ring exception
        }
        if (keyInfo.type != keyInfo.ONE_FILE) {
            //todo throw
        }
    }

    uint32_t ExternalKeyHandler::readData(void *buff, uint32_t len, uint64_t offset) {
        return zeroChunkNode->getChunk(keyId, 0, offset + sizeof(core::KeyInfo), buff, len);
    }
}