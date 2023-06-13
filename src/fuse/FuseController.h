#ifndef RINGSWARM_FUSECONTROLLER_H
#define RINGSWARM_FUSECONTROLLER_H

#include <string>
#include "../core/Id.h"

namespace RingSwarm::fuse {
    void startFuse(std::string &mountPoint);

    void mountRing(core::Id *keyId);

    void stopFuse();
}

#endif //RINGSWARM_FUSECONTROLLER_H
