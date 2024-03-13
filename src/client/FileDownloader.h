#ifndef RINGSWARM_FILEDOWNLOADER_H
#define RINGSWARM_FILEDOWNLOADER_H

#include "../core/Id.h"
#include "KeyHandler.h"

namespace RingSwarm::client {
KeyHandler *createKeyHandler(core::Id *fileId);
}

#endif//RINGSWARM_FILEDOWNLOADER_H
