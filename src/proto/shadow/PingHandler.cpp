#include "../ResponseBuffer.h"
#include "../ServerHandler.h"

namespace RingSwarm::proto {
async::Coroutine<>
ServerHandler::handlePing(ServerResponseState &serverRespState) {
    ResponseBuffer resp(0);
    auto *hdr = reinterpret_cast<ResponseHeader *>(resp.data);
    hdr->tag = serverRespState.requestHeader.tag;
    hdr->responseLen = 0;
    transport->rawWrite(resp.data, resp.len);
    co_return;
}
}// namespace RingSwarm::proto
