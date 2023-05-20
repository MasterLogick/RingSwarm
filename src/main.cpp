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
#include <filesystem>
#include "fuse/FuseController.h"

namespace po = boost::program_options;
using namespace RingSwarm;

int main(int argc, char **argv, char **envp) {
    po::options_description desc("Options");
    std::string host;
    int port;
    int scenario;
    std::string remotePubKey;
    std::string remoteKeyId;
    desc.add_options()
            ("help", "Print this message")
            ("host", po::value(&host)->required()->value_name("address"), "host")
            ("port", po::value(&port)->required()->value_name("number"), "port")
            ("scenario", po::value(&scenario)->required())
            ("pubKey", po::value(&remotePubKey))
            ("keyId", po::value(&remoteKeyId));

    po::positional_options_description p;
    p
            .add("host", 1)
            .add("port", 1)
            .add("scenario", 1)
            .add("pubKey", 1)
            .add("keyId", 1);
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
    std::filesystem::create_directory("./storage");
    std::string fuseMP = "./fuse" + std::to_string(scenario);
    try {
        std::filesystem::create_directory(fuseMP);
    } catch (std::filesystem::filesystem_error &e) {
        system(("fusermount3 -uz " + fuseMP).c_str());
    }
    RingSwarm::fuse::startFuse(fuseMP);

    auto *server = new transport::PlainSocketServer(host, port, 8);
    core::Node::thisNode->connectionInfo = server->getConnectionInfo();
    std::thread([server] { server->listen(); }).detach();

    switch (scenario) {
        case 1: {
            auto d = client::uploadFile("./testFile", 3);
            fuse::mountRing(d);
            break;
        }
        case 2: {
            auto *pubKey = new core::PublicKey();
            boost::algorithm::unhex(remotePubKey, pubKey->begin());
            auto *node = new core::Node(pubKey->getId(), pubKey,
                                        new transport::PlainSocketConnectionInfo("localhost", port - 1));
            core::getOrConnect(node);
            client::getKeyHandler(core::Id::fromHexRepresentation(remoteKeyId.c_str()));
            break;
        }
        default:
            throw core::RingSwarmException();
    }

    std::cin.get();
    storage::closeStorage();
}
