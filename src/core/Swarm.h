#ifndef RINGSWARM_SWARM_H
#define RINGSWARM_SWARM_H

#include <vector>
#include <memory>
#include "Node.h"

namespace RingSwarm::core {
    class Swarm {
    protected:
        std::vector<std::shared_ptr<Node>> swarm;
    public:
        std::vector<std::shared_ptr<Node>> &getSwarmNodes();
    };
}

#endif //RINGSWARM_SWARM_H
