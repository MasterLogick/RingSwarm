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
#include "client/FileDownloader.h"

namespace po = boost::program_options;
using namespace RingSwarm;

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

    std::string path = "ring-swarm-" + std::to_string(scenario) + ".sqlite3";
    storage::loadStorage(path.c_str());
    crypto::loadNodeKeys();

    auto *server = new transport::PlainSocketServer(host, port, 8);
    core::Node::thisNode->connectionInfo = server->getConnectionInfo();
    std::thread([server] { server->listen(); }).detach();

    switch (scenario) {
        case 1: {
            client::uploadFile("RingSwarm", 3);
            break;
        }
        case 2: {
            auto *pubKey = new core::PublicKey();
            boost::algorithm::unhex("030D7A75BAF4CFA1E787EB4BD6D5F4069090A6495D907C28A6A370D980A9AD5EA2",
                                    pubKey->begin());
            auto *node = new core::Node(core::Id::fromHexRepresentation(
                                                "ca298dbf52fbc37e19c06d4e38fd7f7ec2e70bf4c244c684568e9052a646f4a3"),
                                        pubKey,
                                        new transport::PlainSocketConnectionInfo("localhost",
                                                                                 port - 1));
            core::getOrConnect(node);
            client::getKeyHandler(core::Id::fromHexRepresentation(
                    "beb8a3f19205d9c2cf61590a9f955f837e6c2ecf081206bcec694ec8b7de5b55"));
            break;
        }
        default:
            throw core::RingSwarmException();
    }

    std::cin.get();
    storage::closeStorage();
}
