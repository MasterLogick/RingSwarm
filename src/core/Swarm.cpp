#include "Swarm.h"

namespace RingSwarm::core {
    std::vector<std::shared_ptr<Node>> &Swarm::getSwarmNodes() {
        return swarm;
    }
}