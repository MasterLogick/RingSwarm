#ifndef RINGSWARM_SWARM_H
#define RINGSWARM_SWARM_H

#include <vector>
#include <memory>
#include "Node.h"

namespace RingSwarm::core {
    struct Swarm {
        std::vector<Node *> swarm;
    };
}

#endif //RINGSWARM_SWARM_H
