#ifndef RINGSWARM_SRC_PROTO_SERVERRESPONSESTATE_H
#define RINGSWARM_SRC_PROTO_SERVERRESPONSESTATE_H

namespace RingSwarm::proto {
struct ServerResponseState {
    async::Coroutine<> handlerCoro;
    transport::Buffer requestBuffer;
    RequestHeader requestHeader;
};
}// namespace RingSwarm::proto

#endif// RINGSWARM_SRC_PROTO_SERVERRESPONSESTATE_H
