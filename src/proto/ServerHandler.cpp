#include "ServerHandler.h"
#include "TooLargeMessageException.h"
#include "../core/RingSwarmException.h"
#include "RequestHeader.h"
#include "../transport/BoostSocketTransport.h"
#include <boost/log/trivial.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/read.hpp>

#define MAX_REQUEST_LENGTH (1024 * 1024)
namespace RingSwarm::proto {
    using namespace boost::asio;

    awaitable<void> ServerHandler::handleClientConnection() {
        try {
            handleHandshake();
            while (true) {
                RequestHeader requestHeader{};

                co_await async_read(sock, boost::asio::buffer(&requestHeader, 6), use_awaitable);
                if (requestHeader.requestLength > MAX_REQUEST_LENGTH) {
                    throw TooLargeMessageException();
                }
                transport::Buffer buff(requestHeader.requestLength);
                co_await async_read(sock, boost::asio::buffer(buff.getData(), requestHeader.requestLength),
                                    use_awaitable);
                if (requestHeader.method == 0) {
                    throw ProtocolException();
                }
                if (requestHeader.method >= ServerHandler::MethodsCount) {
                    throw ProtocolException();
                }
                BOOST_LOG_TRIVIAL(trace) << "Executing " << ServerHandler::MethodNames[requestHeader.method]
                                         << " method handler";
                co_await (this->*ServerHandler::Methods[requestHeader.method])(buff);
                BOOST_LOG_TRIVIAL(trace) << "Executed " << ServerHandler::MethodNames[requestHeader.method]
                                         << " method handler";
            }
        } catch (core::RingSwarmException &e) {
            BOOST_LOG_TRIVIAL(error) << e.what();
            errorStop();
            co_return;
        }
    }

    ServerHandler::ServerHandler(ip::tcp::socket &&sock) : sock((ip::tcp::socket &&) sock) {
        transport = new transport::BoostSocketTransport(&sock);
    }

    void ServerHandler::errorStop() {
        BOOST_LOG_TRIVIAL(error) << "Error stop in handler";
    }

    boost::asio::awaitable<void>
    ServerHandler::sendNodeListResponse(std::vector<std::shared_ptr<core::Node>> &nodeList) {
        uint32_t nodeListSize = 0;
        for (const auto &node: nodeList) {
            nodeListSize += node->getSerializedSize();
        }
        transport::ResponseBuffer resp(nodeListSize);
        for (const auto &node: nodeList) {
            resp.writeNode(node);
        }
        co_await sock.async_send(buffer(resp.getData(), nodeListSize), use_awaitable);
    }
}