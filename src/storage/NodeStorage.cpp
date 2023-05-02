#include "NodeStorage.h"
#include "Statement.h"
#include "StorageManager.h"

namespace RingSwarm::storage {
    const char *nodeSelect =
            "select pub_key, connection_info\n"
            "from node\n"
            "where node_id = :node_id;";

    core::Node *getNode(core::Id *nodeId) {
        Statement nodeSelectStatement(dbConnection, nodeSelect);
        nodeSelectStatement.bindId(":node_id", nodeId);
        if (!nodeSelectStatement.nextRow()) {
            return nullptr;
        }
        auto *node = new core::Node(nodeId, nodeSelectStatement.getBlob(0), {nodeSelectStatement.getBlob(1)});
        return node;
    }

    const char *nodeInsert =
            "insert into node (node_id, pub_key, connection_info)\n"
            "values (:node_id, :pub_key, :connection_info);";

    void storeNode(core::Node *node) {
        Statement nodeInsertStatement(dbConnection, nodeInsert);
        nodeInsertStatement.bindId(":node_id", node->id);
        nodeInsertStatement.bindBlob(":pub_key", node->publicKey);
        nodeInsertStatement.bindBlob(":connection_info", node->connectionInfo.info);
        nodeInsertStatement.execute();
    }
}