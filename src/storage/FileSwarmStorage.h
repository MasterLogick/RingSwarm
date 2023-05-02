#ifndef RINGSWARM_FILESWARMSTORAGE_H
#define RINGSWARM_FILESWARMSTORAGE_H

#include "../core/Id.h"
#include "../core/FileMetaSwarm.h"

namespace RingSwarm::storage {
    core::FileMetaSwarm *getFileMetaSwarm(core::Id *fileId);

    void storeFileMetaSwarm(core::FileMetaSwarm *fileSwarm);
}
#endif //RINGSWARM_FILESWARMSTORAGE_H
