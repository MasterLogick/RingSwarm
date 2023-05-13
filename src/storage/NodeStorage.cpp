#include "NodeStorage.h"
#include "Statement.h"
#include "StorageManager.h"
#include "ClonedEntityException.h"
#include <map>

namespace RingSwarm::storage {
    std::map<core::Id *, core::Node *, core::Id::Comparator> nodeStorage;
    const char *nodeSelect =
            "select pub_key, connection_info\n"
            "from node\n"
            "where node_id = :node_id;";

    core::Node *getNode(core::Id *nodeId) {
        if (nodeStorage.contains(nodeId)) {
            return nodeStorage[nodeId];
        }
        Statement nodeSelectStatement(dbConnection, nodeSelect);
        nodeSelectStatement.bindId(":node_id", nodeId);
        if (!nodeSelectStatement.nextRow()) {
            return nullptr;
        }
        transport::Buffer b(nodeSelectStatement.getBlob(1));
        auto *node = new core::Node(nodeId, nodeSelectStatement.getBlob(0),
                                    transport::ConnectionInfo::parseConnectionInfo(b));
        nodeStorage[nodeId] = node;
        return node;
    }

    const char *nodeInsert =
            "insert into node (node_id, pub_key, connection_info)\n"
            "values (:node_id, :pub_key, :connection_info);";

    void storeNode(core::Node *node) {
        if (nodeStorage.contains(node->id)) {
            if (nodeStorage[node->id] != node) {
                throw ClonedEntityException();
            }
            return;
        } else {
            nodeStorage[node->id] = node;
        }

        Statement nodeInsertStatement(dbConnection, nodeInsert);
        nodeInsertStatement.bindId(":node_id", node->id);
        nodeInsertStatement.bindBlob(":pub_key", node->publicKey);
        transport::Buffer b(node->connectionInfo->getSerializedSize());
        node->connectionInfo->serialize(b);
        nodeInsertStatement.bindBlob(":connection_info", b.toBlob());
        nodeInsertStatement.execute();
    }

    void storeThisNode() {
        nodeStorage[core::Node::thisNode->id] = core::Node::thisNode;
    }
}