#ifndef RINGSWARM_SRC_TRANSPORT_TRANSPORTEXCEPTION_H
#define RINGSWARM_SRC_TRANSPORT_TRANSPORTEXCEPTION_H

#include "../core/RingSwarmException.h"

namespace RingSwarm::transport {

class TransportException : public core::RingSwarmException {
public:
    TransportException() = default;

    explicit TransportException(std::string reason)
        : core::RingSwarmException(std::move(reason)) {}
};

} // namespace RingSwarm::transport

#endif// RINGSWARM_SRC_TRANSPORT_TRANSPORTEXCEPTION_H
