#ifndef RINGSWARM_FILEUPLOADER_H
#define RINGSWARM_FILEUPLOADER_H

#include "../core/Id.h"

namespace RingSwarm::client {
core::Id *uploadFile(const char *filePath, uint8_t minSwarmSize);
}

#endif//RINGSWARM_FILEUPLOADER_H
