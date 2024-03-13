#ifndef RINGSWARM_CONNECTIONMANAGER_H
#define RINGSWARM_CONNECTIONMANAGER_H

#include "../proto/ClientHandler.h"

namespace RingSwarm::core {
std::shared_ptr<async::Future<proto::ClientHandler *>> getOrConnect(core::Node *node);

std::shared_ptr<async::Future<proto::ClientHandler *>> getOrConnectToOne(std::vector<core::Node *> &nodeList);

template<typename T>
std::shared_ptr<async::Future<proto::ClientHandler *>> getOrConnectToOne(std::map<T, core::Node *> &nodeMap) {
    std::vector<core::Node *> nodeList;
    std::transform(nodeMap.begin(), nodeMap.end(), std::back_inserter(nodeList),
                   [](auto &pair) { return pair.second; });
    return getOrConnectToOne(nodeList);
}

proto::ClientHandler *getPossibleKeyHost(core::Id *, uint8_t index);

}// namespace RingSwarm::core

#endif//RINGSWARM_CONNECTIONMANAGER_H
