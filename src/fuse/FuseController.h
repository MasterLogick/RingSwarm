#ifndef RINGSWARM_FUSECONTROLLER_H
#define RINGSWARM_FUSECONTROLLER_H

#include "../core/Id.h"
#include <string>

namespace RingSwarm::fuse {
void startFuse(std::string &mountPoint);

void mountRing(core::Id *keyId);

void stopFuse();
}// namespace RingSwarm::fuse

#endif//RINGSWARM_FUSECONTROLLER_H
