#include <thread>
#include <iostream>
#include "transport/PlainSocketServer.h"
#include "storage/Statement.h"
#include "crypto/AsymmetricalCrypto.h"
#include "storage/StorageManager.h"
#include "core/RingSwarmException.h"
#include <boost/program_options.hpp>
#include <boost/algorithm/hex.hpp>
#include "client/FileUploader.h"
#include "core/Node.h"
#include "core/ConnectionManager.h"
#include "transport/connectionInfo/PlainSocketConnectionInfo.h"

namespace po = boost::program_options;
namespace RingSwarm {
    void start(int scenario, std::string &host, int port) {
        std::string path = "ring-swarm-" + std::to_string(scenario) + ".sqlite3";
        storage::loadStorage(path.c_str());
        crypto::loadNodeKeys();

        auto *server = new transport::PlainSocketServer(host, port, 8);
        core::Node::thisNode->connectionInfo = server->getConnectionInfo();
        std::thread([server] { server->listen(); }).detach();
    }
}

int main(int argc, char **argv, char **envp) {

    po::options_description desc("Options");
    std::string host;
    int port;
    int scenario;
    desc.add_options()
            ("help", "Print this message")
            ("host", po::value(&host)->required()->value_name("address"), "host")
            ("port", po::value(&port)->required()->value_name("number"), "port")
            ("scenario", po::value(&scenario)->required());

    po::positional_options_description p;
    p
            .add("host", 1)
            .add("port", 1)
            .add("scenario", 1);
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
    try {
        po::notify(vm);
    } catch (boost::program_options::required_option &re) {
        std::cout << re.what() << std::endl << "Usage: RingSwarm [options] host port" << std::endl << desc << std::endl;
        return -1;
    }

    if (vm.count("help")) {
        std::cout << "Usage: RingSwarm [options] host port" << std::endl << desc << std::endl;
        return 0;
    }

    RingSwarm::start(scenario, host, port);
    switch (scenario) {
        case 1: {
            RingSwarm::client::uploadFile("RingSwarm", 3, 5);
            break;
        }
        case 2: {
            std::vector<char> pubKey;
            boost::algorithm::unhex("0337898288E222937A4BDCAEB797C3EE9442F275264DD356269D1252E061C81A40",
                                    std::back_inserter(pubKey));
            auto *node = new RingSwarm::core::Node(RingSwarm::core::Id::fromHexRepresentation(
                                                           "824ec1599c2269df7c8eba87142223cbbb66d141bbaffc13544210697fcc6440"),
                                                   pubKey,
                                                   new RingSwarm::transport::PlainSocketConnectionInfo("localhost",
                                                                                                       port - 1));
            auto *client = RingSwarm::core::getOrConnect(node);
            client->getChunkLink(node->id, 1);
            break;
        }
        default:
            throw RingSwarm::core::RingSwarmException();
    }

    std::cin.get();
    RingSwarm::storage::closeStorage();
}
