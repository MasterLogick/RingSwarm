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
            client::uploadFile("RingSwarm", 3, 5);
            break;
        }
        case 2: {
            auto *pubKey = new crypto::PublicKey();
            boost::algorithm::unhex("03AF34561B00FC80C7254AA1F3A226EDBA3F7A2D055BC581CAE43A004F707166D7",
                                    pubKey->begin());
            auto *node = new core::Node(core::Id::fromHexRepresentation(
                                                "9535dd715563609af32b86271a0521cda381c47c056d3f95bbf61527943805b6"),
                                        pubKey,
                                        new transport::PlainSocketConnectionInfo("localhost",
                                                                                 port - 1));
            core::getOrConnect(node);
            client::getFileHandler(core::Id::fromHexRepresentation(
                    "aaca6d5be6b5f6127da92cb66797f3eb3e52ef6498c3b78f2626c3acbe5df8e9"));
            break;
        }
        default:
            throw core::RingSwarmException();
    }

    std::cin.get();
    storage::closeStorage();
}
