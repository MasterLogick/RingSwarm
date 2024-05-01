#include "FileUploader.h"

#include "../core/ChunkLink.h"
#include "../core/KeyInfo.h"
#include "../crypto/HashCrypto.h"
#include "../storage/ChunkStorage.h"
#include "../storage/ChunkSwarmStorage.h"
#include "../storage/KeySwarmStorage.h"

#include <boost/log/trivial.hpp>

#include <filesystem>

namespace RingSwarm::client {
core::Id *uploadFile(const char *filePath, uint8_t minSwarmSize) {
    if (!std::filesystem::exists(filePath) ||
        !std::filesystem::exists("storage")) {
        throw core::RingSwarmException();
    }
    crypto::KeyPair keyPair;
    auto *keyId = keyPair.publicKey->getId();
    std::string chunkPath =
        std::string("storage/") + keyId->getHexRepresentation() + ".0";
    auto externalFile = fopen(filePath, "r");
    auto chunkFile = fopen(chunkPath.c_str(), "w");
    core::KeyInfo keyInfo{};
    keyInfo.type = core::KeyInfo::ONE_FILE;
    keyInfo.chunksCount = 1;
    fwrite(&keyInfo, sizeof(core::KeyInfo), 1, chunkFile);
    char buff[8192];
    int read = 0;
    while ((read = fread(buff, 1, 8192, externalFile)) > 0) {
        fwrite(buff, 1, read, chunkFile);
    }
    fclose(externalFile);
    fclose(chunkFile);
    auto chunk = storage::getMappedChunk(keyId, 0);
    auto *dataHash = crypto::hashData(chunk->getData(), chunk->getSize());
    auto *link = core::ChunkLink::createChunkLink(
        keyPair,
        0,
        dataHash,
        chunk->getSize()
    );
    std::map<uint8_t, core::Node *> keySwarmNodes;
    for (int i = 0; i < minSwarmSize; ++i) {
        keySwarmNodes[i] = core::Node::thisNode;
    }
    auto *ring = new core::ChunkRing();
    (*ring)[0].push_back(core::Node::thisNode);
    auto *keySwarm =
        new core::KeySwarm(keyId, keyPair.publicKey, keySwarmNodes, ring);
    auto *chunkSwarm = new core::ChunkSwarm(link, ring);
    storage::storeKeySwarm(keySwarm);
    storage::storeChunkSwarm(chunkSwarm);
    return keyId;
}
}// namespace RingSwarm::client
