#ifndef RINGSWARM_CONNECTIONSSTORAGE_H
#define RINGSWARM_CONNECTIONSSTORAGE_H

#include <memory>
#include "../core/Id.h"
#include "../core/Node.h"

namespace RingSwarm::storage {
    std::shared_ptr<core::Node> getPossibleFileMetaHost(core::Id *fileId, uint8_t index);
}
#endif //RINGSWARM_CONNECTIONSSTORAGE_H
