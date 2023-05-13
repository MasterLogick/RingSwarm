#ifndef RINGSWARM_NODESTORAGE_H
#define RINGSWARM_NODESTORAGE_H

#include "../core/Node.h"

namespace RingSwarm::storage {
    core::Node *getNode(core::Id *nodeId);

    void storeNode(core::Node *node);

    void storeThisNode();
}

#endif //RINGSWARM_NODESTORAGE_H
