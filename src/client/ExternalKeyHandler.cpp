#include "ExternalKeyHandler.h"

#include "../core/ConnectionManager.h"

#include <boost/log/trivial.hpp>

namespace RingSwarm::client {

#define CACHE_SIZE 20
#define CACHE_OFFSET_MASK ((1 << CACHE_SIZE) - 1)

ExternalKeyHandler::ExternalKeyHandler(core::Id *keyId, core::PublicKey *key,
                                       proto::ClientHandler *possibleKeySwarmNode) : keyId(keyId), key(key) {
    //todo clean up resolved futures
    auto keySwarm = std::get<0>(possibleKeySwarmNode->getKeySwarm(keyId)->await());
    keySwarmNode = std::get<0>(core::getOrConnectToOne(keySwarm)->await());
    if (keySwarmNode == nullptr) {
        //todo throw and search for next entry point
    }
    auto zeroChunkSwarm = std::get<0>(keySwarmNode->getChunkSwarm(keyId, 0)->await());
    zeroChunkNode = std::get<0>(core::getOrConnectToOne(zeroChunkSwarm)->await());

    zeroChunk = std::get<0>(zeroChunkNode->getChunkLink(keyId, 0)->await());
    auto readSize = std::get<0>(zeroChunkNode->getChunk(keyId, 0, 0, &keyInfo, sizeof(core::KeyInfo))->await());
    if (readSize != sizeof(keyInfo)) {
        BOOST_LOG_TRIVIAL(trace) << "malformed chunk";
    }
    if (keyInfo.type != keyInfo.ONE_FILE) {
        BOOST_LOG_TRIVIAL(trace) << "unsupported chunk type";
    }
}

std::shared_ptr<async::Future<void *, uint32_t>> ExternalKeyHandler::readData(uint32_t len, uint64_t offset) {
    uint64_t part = offset >> CACHE_SIZE;
    auto iter = data.find(part);
    if (iter != data.end()) {
        uint32_t off = offset & CACHE_OFFSET_MASK;
        return async::Future<void *, uint32_t>::createResolved(iter->second + off,
                                                               std::min<uint32_t>((1 << CACHE_SIZE) - off, len));
    } else {
        auto *ptr = data[part] = new uint8_t[1 << CACHE_SIZE];
        auto f = async::Future<void *, uint32_t>::create();
        zeroChunkNode->getChunk(keyId, 0, (part << CACHE_SIZE) + sizeof(core::KeyInfo), ptr,
                                1 << CACHE_SIZE)
                ->then([f, ptr, offset, len](uint32_t realSize) {
                    uint32_t off = offset & CACHE_OFFSET_MASK;
                    f->resolve(ptr + off, std::min<uint32_t>(realSize - off, len));
                });
        return f;
    }
}

ExternalKeyHandler::~ExternalKeyHandler() {
}

uint64_t ExternalKeyHandler::getDataSize() {
    return zeroChunk->dataSize - sizeof(core::KeyInfo);
}
}// namespace RingSwarm::client