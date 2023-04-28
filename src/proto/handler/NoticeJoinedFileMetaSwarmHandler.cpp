#include "../ServerHandler.h"
#include "../../storage/FileSwarmStorage.h"
#include "../ClientHandler.h"

#define MAX_RESPONSE_SIZE (256 * 8 * 1024)
namespace RingSwarm::proto {
    boost::asio::awaitable<std::vector<core::Node *>> ClientHandler::noticeJoinedFileMetaSwarm(core::Id &fileId) {
        transport::RequestBuffer req(32);
        req.writeId(fileId);
        co_await transport->sendRequest(6, req);
        auto resp = co_await transport->readResponse(MAX_RESPONSE_SIZE);
        co_return resp.readNodeList();
    }

    boost::asio::awaitable<void> ServerHandler::handleNoticeJoinedFileMetaSwarm(transport::Buffer &request) {
        auto id = request.readId();
        auto fileMetaSwarm = storage::getHostedFileMetaSwarm(id);
        if (fileMetaSwarm == nullptr) {
            co_await transport->sendError();
        } else {
            auto nodeList = fileMetaSwarm->getSwarmNodes();
            if (std::none_of(nodeList.begin(), nodeList.end(),
                             [&](auto node) -> bool { return node == remote; })) {
                nodeList.push_back(remote);
            }
            co_await sendNodeListResponse(nodeList);
        }
    }
}