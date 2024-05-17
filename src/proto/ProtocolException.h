#ifndef RINGSWARM_PROTOCOLEXCEPTION_H
#define RINGSWARM_PROTOCOLEXCEPTION_H

#include "../core/RingSwarmException.h"

namespace RingSwarm::proto {
class ProtocolException : public core::RingSwarmException {
public:
    ProtocolException() = default;

    explicit ProtocolException(std::string reason)
        : core::RingSwarmException(std::move(reason)) {}
};
}// namespace RingSwarm::proto

#endif// RINGSWARM_PROTOCOLEXCEPTION_H
