#ifndef RINGSWARM_SRC_TRANSPORT_CLOSEDTRANSPORTEXCEPTION_H
#define RINGSWARM_SRC_TRANSPORT_CLOSEDTRANSPORTEXCEPTION_H

#include "TransportException.h"

namespace RingSwarm::transport {
class ClosedTransportException : public TransportException {
public:
    ClosedTransportException() = default;

    explicit ClosedTransportException(std::string reason) : TransportException(std::move(reason)) {}
};

}// namespace RingSwarm::transport
#endif//RINGSWARM_SRC_TRANSPORT_CLOSEDTRANSPORTEXCEPTION_H
