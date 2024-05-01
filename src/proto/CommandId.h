#ifndef RINGSWARM_SRC_PROTO_COMMANDID_H
#define RINGSWARM_SRC_PROTO_COMMANDID_H

#include <cstdint>

namespace RingSwarm::proto {
enum CommandId : uint8_t {
    CommandId_End = 0,
    CommandId_GetKey = 1,
    CommandId_GetNearestChunk = 2,
    CommandId_GetChunkLink = 3,
    CommandId_GetChunk = 4,
    CommandId_DragIntoKeySwarm = 5,
    CommandId_NoticeJoinedKeySwarm = 6,
    CommandId_NoticeJoinedChunkSwarm = 7,
    CommandId_GetKeySwarm = 8,
    CommandId_GetChunkSwarm = 9,
    CommandId_NoticeLeavedChunkSwarm = 10,
    CommandId_SubscribeOnChunkChange = 11,
    CommandId_ChunkChangeEvent = 12,
    CommandId_UnsubscribeOnChunkChange = 13,
    CommandId_COMMAND_COUNT = 14,
};

constexpr static const char *MethodNames[] = {
    nullptr,
    "GetKey",
    "GetNearestChunk",
    "GetChunkLink",
    "GetChunk",
    "DragIntoKeySwarm",
    "NoticeJoinedKeySwarm",
    "NoticeJoinedChunkSwarm",
    "GetKeySwarm",
    "GetChunkSwarm",
    "NoticeLeavedChunkSwarm",
    "SubscribeOnChunkChange",
    "ChunkChangeEvent",
    "UnsubscribeOnChunkChange"
};

}// namespace RingSwarm::proto

#endif// RINGSWARM_SRC_PROTO_COMMANDID_H
