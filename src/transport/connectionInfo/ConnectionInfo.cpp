#include "ConnectionInfo.h"
#include "../DataSerialisationException.h"
#include "PlainSocketConnectionInfo.h"

namespace RingSwarm::transport {
typedef ConnectionInfo *(*Parser)(Buffer &buffer);

constexpr Parser Parsers[] = {
        nullptr,
        &PlainSocketConnectionInfo::parseConnectionInfo};
constexpr uint16_t ParsersCount = 2;
static_assert(sizeof(Parsers) == sizeof(Parser) * ParsersCount);

ConnectionInfo *ConnectionInfo::parseConnectionInfo(Buffer &buffer) {
    uint8_t type = buffer.read<uint8_t>();
    if (type >= ParsersCount) {
        throw DataSerialisationException();
    }
    return Parsers[type](buffer);
}
}// namespace RingSwarm::transport
