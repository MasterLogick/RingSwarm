#include "FileUploader.h"
#include "../core/ChunkLink.h"
#include "../crypto/HashCrypto.h"
#include "../storage/ChunkSwarmStorage.h"
#include "../storage/KeySwarmStorage.h"
#include <fstream>
#include <chrono>

namespace RingSwarm::client {
    core::Id *uploadFile(const char *filePath, uint8_t minSwarmSize, uint8_t ringConnectivity) {
        std::ifstream input(filePath, std::ios::binary);
        std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(input), {});
/*        auto *key = core::FileMeta::createNewFileMeta(1, buffer.size(), minSwarmSize, ringConnectivity);
        auto *dataHash = crypto::hashData(buffer.data(), buffer.size());
        auto *link = core::ChunkLink::createChunkLink(meta->fileId, 0, dataHash);
        std::map<int, core::Node *> fileMetaSwarmNodes;
        for (int i = 0; i < minSwarmSize; ++i) {
            fileMetaSwarmNodes[i] = core::Node::thisNode;
        }
        auto *ring = new core::ChunkRing();
        (*ring)[0].push_back(core::Node::thisNode);
        auto *fileMetaSwarm = new core::KeySwarm(meta, fileMetaSwarmNodes, ring);
        auto *chunkSwarm = new core::ChunkSwarm(link, ring);
        storage::storeKeySwarm(fileMetaSwarm);
        storage::storeChunkSwarm(chunkSwarm);
        return fileMetaSwarm->meta->fileId;*/
    }
}