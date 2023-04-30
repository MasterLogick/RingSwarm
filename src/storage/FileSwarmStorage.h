#ifndef RINGSWARM_FILESWARMSTORAGE_H
#define RINGSWARM_FILESWARMSTORAGE_H

#include "../core/Id.h"
#include "../core/FileMetaSwarm.h"

namespace RingSwarm::storage {
    core::FileMetaSwarm *getHostedFileMetaSwarm(core::Id *fileId);

    void storeNewFileMetaSwarm(std::shared_ptr<core::FileMetaSwarm> &fileSwarm);
}
#endif //RINGSWARM_FILESWARMSTORAGE_H
