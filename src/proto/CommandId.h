#ifndef RINGSWARM_SRC_PROTO_COMMANDID_H
#define RINGSWARM_SRC_PROTO_COMMANDID_H

#include <cstdint>

namespace RingSwarm::proto {
enum CommandId : uint8_t {
    CommandId_End = 0,
    CommandId_Ping = 1,
    CommandId_GetNode = 2,
    CommandId_COMMAND_COUNT = 3,
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
