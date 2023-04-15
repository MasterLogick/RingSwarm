#ifndef RINGSWARM_FILESWARMSTORAGE_H
#define RINGSWARM_FILESWARMSTORAGE_H

#include "../core/Id.h"
#include "../core/FileMetaSwarm.h"

namespace RingSwarm::storage {
    core::FileMetaSwarm *getHostedFileMetaSwarm(core::Id *id);

    void storeNewFileMetaSwarm(std::shared_ptr<core::FileMetaSwarm> sharedPtr);
}
#endif //RINGSWARM_FILESWARMSTORAGE_H
