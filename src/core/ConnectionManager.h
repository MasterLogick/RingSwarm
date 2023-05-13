#ifndef RINGSWARM_CONNECTIONMANAGER_H
#define RINGSWARM_CONNECTIONMANAGER_H

#include "../proto/ClientHandler.h"

namespace RingSwarm::core {
    proto::ClientHandler *getOrConnect(core::Node *node);

    core::Node *getPossibleFileMetaHost(core::Id *, uint8_t index);

} // core

#endif //RINGSWARM_CONNECTIONMANAGER_H
