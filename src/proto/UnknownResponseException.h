#ifndef RINGSWARM_SRC_PROTO_UNKNOWNRESPONSEEXCEPTION_H
#define RINGSWARM_SRC_PROTO_UNKNOWNRESPONSEEXCEPTION_H

#include "ProtocolException.h"

namespace RingSwarm::proto {
class UnknownResponseException : public ProtocolException {
public:
    UnknownResponseException() = default;

    explicit UnknownResponseException(std::string reason)
        : ProtocolException(std::move(reason)) {}
};
} // namespace RingSwarm::proto

#endif // RINGSWARM_SRC_PROTO_UNKNOWNRESPONSEEXCEPTION_H
