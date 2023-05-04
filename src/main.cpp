#include <thread>
#include <iostream>
#include "transport/PlainSocketServer.h"
#include "storage/Statement.h"
#include "crypto/AsymetricalCrypto.h"
#include "storage/StorageManager.h"
#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char **argv, char **envp) {

    po::options_description desc("Options");
    std::string host;
    int port;
    desc.add_options()
            ("help", "Print this message")
            ("host", po::value(&host)->required()->value_name("address"), "host")
            ("port", po::value(&port)->required()->value_name("number"), "port");

    po::positional_options_description p;
    p
            .add("host", 1)
            .add("port", 1);
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

    RingSwarm::storage::loadStorage("ring-swarm.sqlite3");
    RingSwarm::crypto::loadNodeKeys();

    RingSwarm::transport::PlainSocketServer server(host, port, 8);
    std::thread serv([&server] { server.listen(); });

    std::cin.get();
    RingSwarm::storage::closeStorage();
}
