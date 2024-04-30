#ifndef RINGSWARM_SRC_PROTO_CLIENTREQUESTSTATE_H
#define RINGSWARM_SRC_PROTO_CLIENTREQUESTSTATE_H

#include "../async/coroutine.h"
#include "ResponseHeader.h"
#include <coroutine>
namespace RingSwarm {
namespace proto {

struct ClientRequestState {
    ResponseHeader header;
    std::coroutine_handle<async::Promise<ResponseHeader>> handle;
    uint32_t maxResponseSize;
    uint16_t tag;
};

}// namespace proto
}// namespace RingSwarm

#endif//RINGSWARM_SRC_PROTO_CLIENTREQUESTSTATE_H
