#include "FileSwarmStorage.h"
#include "StorageManager.h"
#include "Statement.h"
#include "StorageException.h"
#include <boost/log/trivial.hpp>

namespace RingSwarm::storage {
    const char *getHostedFileMetaSwarmByFileIdStatement =
            "select file_meta_id,\n"
            "       author,\n"
            "       creation_timestamp,\n"
            "       chunks_count,\n"
            "       chunk_size,\n"
            "       min_swarm_size,\n"
            "       ring_connectivity,\n"
            "       opts,\n"
            "       sign,\n"
            "       file_id,\n"
            "       swarm_index\n"
            "from file_meta\n"
            "         join file_swarm on file_meta.id = file_swarm.file_meta_id\n"
            "where file_id = :file_id\n"
            "  and node_id = :node_id;";

    core::FileMetaSwarm *getHostedFileMetaSwarm(core::Id *fileId) {
        Statement getSwarmById(dbConnection, getHostedFileMetaSwarmByFileIdStatement);
        getSwarmById.bindId(":file_id", fileId);
        getSwarmById.bindInt64(":node_id", core::Node::thisNode->internalStorageId);
        if (getSwarmById.nextRow()) {
            auto fileMeta = new core::FileMeta(
                    getSwarmById.getInt64(0),
                    getSwarmById.getId(1),
                    getSwarmById.getInt64(2),
                    getSwarmById.getInt64(3),
                    getSwarmById.getInt32(4),
                    getSwarmById.getInt32(5),
                    getSwarmById.getInt32(6),
                    //todo read opts
                    {}, // getSwarmById.getText(7)
                    getSwarmById.getBlob(8),
                    getSwarmById.getId(9)
            );
            return new core::FileMetaSwarm(fileMeta, getSwarmById.getInt32(10));
        } else {
            return nullptr;
        }
    }

    const char *insertFileMeta =
            "INSERT INTO file_meta (file_id,\n"
            "                       author,\n"
            "                       creation_timestamp,\n"
            "                       chunks_count,\n"
            "                       chunk_size,\n"
            "                       min_swarm_size,\n"
            "                       ring_connectivity,\n"
            "                       opts,\n"
            "                       sign)\n"
            "VALUES (:file_id,\n"
            "        :author,\n"
            "        :creation_timestamp,\n"
            "        :chunks_count,\n"
            "        :chunk_size,\n"
            "        :min_swarm_size,\n"
            "        :ring_connectivity,\n"
            "        :opts,\n"
            "        :sign)\n"
            "returning id;";
    const char *insertHostedFileSwarm =
            "insert into file_swarm (file_meta_id,\n"
            "                        swarm_index,\n"
            "                        node_id)\n"
            "VALUES (:file_meta_id,\n"
            "        :swarm_index,\n"
            "        :node_id)";

    void storeNewFileMetaSwarm(std::shared_ptr<core::FileMetaSwarm> &fileSwarm) {
        auto *fileMeta = fileSwarm->meta;
        Statement insertFileMetaStatement(dbConnection, insertFileMeta);
        insertFileMetaStatement.bindId(":file_id", fileMeta->fileId);
        insertFileMetaStatement.bindId(":author", fileMeta->author);
        insertFileMetaStatement.bindInt64(":creation_timestamp", fileMeta->creationTimestamp);
        insertFileMetaStatement.bindInt64(":chunks_count", fileMeta->chunksCount);
        insertFileMetaStatement.bindInt32(":chunk_size", fileMeta->chunkSize);
        insertFileMetaStatement.bindInt32(":min_swarm_size", fileMeta->minSwarmSize);
        insertFileMetaStatement.bindInt32(":ring_connectivity", fileMeta->ringConnectivity);
        insertFileMetaStatement.bindBlob(":opts", fileMeta->optMeta, 10);
        insertFileMetaStatement.bindBlob(":sign", fileMeta->sign);
        if (insertFileMetaStatement.nextRow()) {
            fileMeta->internalStorageId = insertFileMetaStatement.getInt64(0);
        } else {
            BOOST_LOG_TRIVIAL(error) << "file meta insert failed";
            throw StorageException();
        }
        Statement insertHostedFileSwarmStatement(dbConnection, insertHostedFileSwarm);
        auto &swarm = fileSwarm->swarm;
        for (int i = 0; i < swarm.size(); ++i) {
            insertHostedFileSwarmStatement.bindInt64(":file_meta_id", fileMeta->internalStorageId);
            insertHostedFileSwarmStatement.bindInt32(":swarm_index", i);
            insertHostedFileSwarmStatement.bindInt64(":node_id", swarm[i]->internalStorageId);
            insertHostedFileSwarmStatement.nextRow();
            insertHostedFileSwarmStatement.reset();
        }
    }
}