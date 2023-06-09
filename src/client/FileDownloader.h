#ifndef RINGSWARM_FILEDOWNLOADER_H
#define RINGSWARM_FILEDOWNLOADER_H

#include "KeyHandler.h"
#include "../core/Id.h"

namespace RingSwarm::client {
    KeyHandler *createKeyHandler(core::Id *fileId);
}

#endif //RINGSWARM_FILEDOWNLOADER_H
