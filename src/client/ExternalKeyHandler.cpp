#include "ExternalKeyHandler.h"
#include "../core/ConnectionManager.h"
#include <boost/log/trivial.hpp>

namespace RingSwarm::client {
    ExternalKeyHandler::ExternalKeyHandler(core::Id *keyId, core::PublicKey *key,
                                           proto::ClientHandler *possibleKeySwarmNode) :
            keyId(keyId), key(key) {
        //todo clean up resolved futures
        auto keySwarm = std::get<0>(possibleKeySwarmNode->getKeySwarm(keyId)->await());
        keySwarmNode = std::get<0>(core::getOrConnectToOne(keySwarm)->await());
        if (keySwarmNode == nullptr) {
            //todo throw and search for next entry point
        }
        auto zeroChunkSwarm = std::get<0>(keySwarmNode->getChunkSwarm(keyId, 0)->await());
        zeroChunkNode = std::get<0>(core::getOrConnectToOne(zeroChunkSwarm)->await());

        auto readSize = std::get<0>(zeroChunkNode->getChunk(keyId, 0, 0, &keyInfo, sizeof(core::KeyInfo))->await());
        if (readSize != sizeof(keyInfo)) {
            BOOST_LOG_TRIVIAL(trace) << "malformed chunk";
        }
        if (keyInfo.type != keyInfo.ONE_FILE) {
            BOOST_LOG_TRIVIAL(trace) << "unsupported chunk type";
        }
    }

    uint32_t ExternalKeyHandler::readData(void *buff, uint32_t len, uint64_t offset) {
        return std::get<0>(zeroChunkNode->getChunk(keyId, 0, offset + sizeof(core::KeyInfo), buff, len)->await());
    }

    ExternalKeyHandler::~ExternalKeyHandler() {

    }
}