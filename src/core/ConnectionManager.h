#ifndef RINGSWARM_CONNECTIONMANAGER_H
#define RINGSWARM_CONNECTIONMANAGER_H

#include "../proto/ClientHandler.h"

namespace RingSwarm::core {
    proto::ClientHandler *getOrConnect(core::Node *node);

    proto::ClientHandler *getPossibleFileMetaHost(core::Id *, uint8_t index);

}

#endif //RINGSWARM_CONNECTIONMANAGER_H
