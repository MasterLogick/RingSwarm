#include "FileSwarmStorage.h"
#include "StorageManager.h"
#include "Statement.h"
#include "StorageException.h"
#include "ChunkRingStorage.h"
#include "NodeStorage.h"

namespace RingSwarm::storage {
    std::map<core::Id *, core::FileMetaSwarm *, core::Id::Comparator> fileMetaSwarmStorage;
    const char *fileMetaSelect =
            "select author,\n"
            "       creation_timestamp,\n"
            "       chunks_count,\n"
            "       chunk_size,\n"
            "       min_swarm_size,\n"
            "       ring_connectivity,\n"
            "       opts,\n"
            "       sign\n"
            "from file_meta\n"
            "where file_id = :file_id;";
    const char *fileSwarmSelect =
            "select swarm_index, node_id\n"
            "from file_swarm\n"
            "where file_id = :file_id;";

    core::FileMetaSwarm *getFileMetaSwarm(core::Id *fileId) {
        if (fileMetaSwarmStorage.contains(fileId)) {
            return fileMetaSwarmStorage[fileId];
        }
        Statement fileMetaSelectStatement(dbConnection, fileMetaSelect);
        fileMetaSelectStatement.bindId(":file_id", fileId);
        if (!fileMetaSelectStatement.nextRow()) {
            return nullptr;
        }
        auto fileMeta = new core::FileMeta(
                fileMetaSelectStatement.getId(0),
                fileMetaSelectStatement.getInt64(1),
                fileMetaSelectStatement.getInt64(2),
                fileMetaSelectStatement.getInt32(3),
                fileMetaSelectStatement.getInt32(4),
                fileMetaSelectStatement.getInt32(5),
                //todo read opts
                {}, // getSwarmById.getText(6)
                fileMetaSelectStatement.getBlob(7),
                fileId
        );

        auto *metaSwarm = new core::FileMetaSwarm(fileMeta, {}, getChunkRing(fileId));
        auto &swarm = metaSwarm->swarm;
        Statement fileSwarmSelectStatement(dbConnection, fileSwarmSelect);
        fileSwarmSelectStatement.bindId(":file_id", fileId);
        while (fileSwarmSelectStatement.nextRow()) {
            auto *nodeId = fileSwarmSelectStatement.getId(1);
            auto *node = getNode(nodeId);
            if (node != nullptr) {
                delete nodeId;
            }
            swarm[fileSwarmSelectStatement.getInt32(0)] = node;
        }
        fileMetaSwarmStorage[fileId] = metaSwarm;
        return metaSwarm;
    }

    const char *fileMetaInsert =
            "insert into file_meta (file_id, author, creation_timestamp, chunks_count, chunk_size, min_swarm_size, ring_connectivity,\n"
            "                       opts, sign)\n"
            "values (:file_id, :author, :creation_timestamp, :chunks_count, :chunk_size, :min_swarm_size, :ring_connectivity,\n"
            "        :opts, :sign);";
    const char *fileSwarmInsert =
            "insert into file_swarm(file_id, swarm_index, node_id)\n"
            "values (:file_id, :swarm_index, :node_id);";

    void storeFileMetaSwarm(core::FileMetaSwarm *fileSwarm) {
        if (fileMetaSwarmStorage.contains(fileSwarm->meta->fileId)) {
            throw StorageException();
        } else {
            fileMetaSwarmStorage[fileSwarm->meta->fileId] = fileSwarm;
        }

        auto *fileMeta = fileSwarm->meta;
        Statement fileMetaInsertStatement(dbConnection, fileMetaInsert);
        fileMetaInsertStatement.bindId(":file_id", fileMeta->fileId);
        fileMetaInsertStatement.bindId(":author", fileMeta->author);
        fileMetaInsertStatement.bindInt64(":creation_timestamp", fileMeta->creationTimestamp);
        fileMetaInsertStatement.bindInt64(":chunks_count", fileMeta->chunksCount);
        fileMetaInsertStatement.bindInt32(":chunk_size", fileMeta->chunkSize);
        fileMetaInsertStatement.bindInt32(":min_swarm_size", fileMeta->minSwarmSize);
        fileMetaInsertStatement.bindInt32(":ring_connectivity", fileMeta->ringConnectivity);
        fileMetaInsertStatement.bindBlob(":opts", fileMeta->optMeta, 10);
        fileMetaInsertStatement.bindBlob(":sign", fileMeta->sign);
        fileMetaInsertStatement.execute();

        Statement fileSwarmInsertStatement(dbConnection, fileSwarmInsert);
        fileSwarmInsertStatement.bindId(":file_id", fileMeta->fileId);
        auto &swarm = fileSwarm->swarm;
        for (const auto &item: swarm) {
            fileSwarmInsertStatement.bindId(":node_id", item.second->id);
            fileSwarmInsertStatement.bindInt32(":swarm_index", item.first);
            fileSwarmInsertStatement.execute();
            fileSwarmInsertStatement.reset();
        }

        storeChunkRing(fileMeta->fileId, fileSwarm->ring);

    }
}