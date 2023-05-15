#include "FileUploader.h"
#include "../core/ChunkLink.h"
#include "../crypto/HashCrypto.h"
#include "../storage/ChunkSwarmStorage.h"
#include "../storage/KeySwarmStorage.h"
#include <fstream>
#include <chrono>

namespace RingSwarm::client {
    core::Id *uploadFile(const char *filePath, uint8_t minSwarmSize) {
        std::ifstream input(filePath, std::ios::binary);
        std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(input), {});
        crypto::KeyPair keyPair;
        auto *dataHash = crypto::hashData(buffer);
        auto *link = core::ChunkLink::createChunkLink(keyPair, 0, dataHash);
        auto *keyId = keyPair.publicKey->getId();
        std::map<int, core::Node *> keySwarmNodes;
        for (int i = 0; i < minSwarmSize; ++i) {
            keySwarmNodes[i] = core::Node::thisNode;
        }
        auto *ring = new core::ChunkRing();
        (*ring)[0].push_back(core::Node::thisNode);
        auto *keySwarm = new core::KeySwarm(keyId, keyPair.publicKey, keySwarmNodes, ring);
        auto *chunkSwarm = new core::ChunkSwarm(link, ring);
        storage::storeKeySwarm(keySwarm);
        storage::storeChunkSwarm(chunkSwarm);
        return keyId;
    }
}