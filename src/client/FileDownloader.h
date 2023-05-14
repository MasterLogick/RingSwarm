#ifndef RINGSWARM_FILEDOWNLOADER_H
#define RINGSWARM_FILEDOWNLOADER_H

#include "FileHandler.h"
#include "../core/Id.h"

namespace RingSwarm::client {
    FileHandler *getFileHandler(core::Id *fileId);
}

#endif //RINGSWARM_FILEDOWNLOADER_H
