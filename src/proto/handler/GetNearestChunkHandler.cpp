#include <map>
#include <vector>
#include "../ServerHandler.h"
#include "../../storage/FileSwarmStorage.h"
#include "../../core/Random.h"
#include "../../storage/ChunkSwarmStorage.h"
#include "../ClientHandler.h"

#define MAX_RESPONSE_SIZE (1024*8)
namespace RingSwarm::proto {

    uint64_t dist(const uint64_t &lhs, const uint64_t &rhs) {
        return lhs > rhs ? lhs - rhs : rhs - lhs;
    }

    boost::asio::awaitable<void> sendNearestChunkSwarm(transport::Transport *transport, uint64_t chunkIndex,
                                                       std::map<uint64_t, std::vector<std::shared_ptr<core::Node>>> &connectedSwarms) {
        uint64_t nearestSwarm = connectedSwarms.begin()->first;
        for (const auto &chunkSwarm: connectedSwarms) {
            if (dist(chunkSwarm.first, chunkIndex) < dist(nearestSwarm, chunkIndex)) {
                nearestSwarm = chunkSwarm.first;
            }
        }
        auto &swarm = connectedSwarms[nearestSwarm];
        auto &node = swarm[core::getRandom(swarm.size())];
        transport::ResponseBuffer resp(8 + node->getSerializedSize());
        resp.writeUint64(nearestSwarm);
        resp.writeNode(node);
        co_await transport->sendResponse(resp);
    }

    boost::asio::awaitable<uint64_t>
    ClientHandler::getNearestChunk(core::Id &fileId, uint64_t chunkIndex, core::Node **node) {
        transport::RequestBuffer req(32 + 8);
        req.writeId(fileId);
        req.writeUint64(chunkIndex);
        co_await transport->sendRequest(2, req);
        auto resp = co_await transport->readResponse(MAX_RESPONSE_SIZE);
        auto swarmIndex = resp.readUint64();
        *node = resp.readNode();
        co_return swarmIndex;
    }

    boost::asio::awaitable<void> ServerHandler::handleGetNearestChunk(transport::Buffer &request) {
        auto fileId = request.readId();
        auto chunkIndex = request.readUint64();
        auto fileMetaSwarm = storage::getHostedFileMetaSwarm(fileId);
        //todo select best swarm from both cases if they exist
        if (fileMetaSwarm == nullptr) {
            auto chunkSwarm = storage::getNearestHostedChunkSwarm(fileId, chunkIndex);
            if (chunkSwarm == nullptr) {
                co_await transport->sendError();
            } else {
                auto &connectedSwarms = chunkSwarm->getRingConnections();
                co_await ::RingSwarm::proto::sendNearestChunkSwarm(transport, chunkIndex, connectedSwarms);
            }
        } else {
            auto &connectedSwarms = fileMetaSwarm->getConnectedChunkSwarms();
            co_await ::RingSwarm::proto::sendNearestChunkSwarm(transport, chunkIndex, connectedSwarms);
        }
    }
}