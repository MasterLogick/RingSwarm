#include "async/EventLoop.h"
#include "async/ThreadPool.h"
#include "core/NodeContext.h"
#include "proto/RequestHeader.h"
#include "transport/PlainSocketTransport.h"

using namespace RingSwarm;

async::Coroutine<> test() {
    transport::PlainSocketTransport client;
    int conn = co_await client.connect("localhost", 12345);
    if (conn != 0) {
        co_return;
    }
    proto::RequestHeader rh{
        .requestLength = 0,
        .method = RingSwarm::proto::CommandId_GetKey,
        .tag = 1
    };
    client.rawWrite(&rh, sizeof(rh));

//    char c;
//    co_await client.rawRead(&c, 1);
//    BOOST_LOG_TRIVIAL(trace) << "Read data: " << c;
}

int main(int argc, char **argv, char **envp) {
    async::ThreadPool::setDefaultThreadPool(new async::ThreadPool(1));
    async::EventLoop::setMainEventLoop(new async::EventLoop());
    auto mainEventLoopRunRes = async::EventLoop::getMainEventLoop()->run();
    if (!mainEventLoopRunRes) {
        BOOST_LOG_TRIVIAL(error) << "Main event loop initialization failed";
        return -1;
    }

    core::NodeContext ctx;
    ctx.addServer("0.0.0.0", 12345);

    //    transport::PlainSocketServer server("0.0.0.0", 12345);
    //    int err =
//    server.listen([](std::unique_ptr<transport::PlainSocketTransport>
//    sock) {
    //        std::cout << "Connected!!!" << std::endl;
    //        char c = '#';
    //        sock->rawWrite(&c, 1);
    //    });
    //    if (err) {
    //        BOOST_LOG_TRIVIAL(error) << "Server listen error: " << err;
    //        return -1;
    //    }
    async::Coroutine<> c = test();
    while (!c.getHandle().done()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    async::ThreadPool::getDefaultThreadPool()->waitEmpty();
    async::EventLoop::getMainEventLoop()->stop();
    async::ThreadPool::getDefaultThreadPool()->blockingStop();
    BOOST_LOG_TRIVIAL(trace) << "end";
    /*core::setThreadName("RingSwarm");
    po::options_description desc("Options");
    std::string host;
    int port;
    int scenario;
    std::string remotePubKey;
    desc.add_options()("help", "Print this message")("host",
po::value(&host)->required()->value_name("address"), "host")("port",
po::value(&port)->required()->value_name("number"), "port")("scenario",
po::value(&scenario)->required())("pubKey", po::value(&remotePubKey));

    po::positional_options_description p;
    p
            .add("host", 1)
            .add("port", 1)
            .add("scenario", 1)
            .add("pubKey", 1);
    po::variables_map vm;
    po::store(po::command_line_parser(argc,
argv).options(desc).positional(p).run(), vm); try { po::notify(vm); } catch
(boost::program_options::required_option &re) { std::cout << re.what() <<
std::endl
                  << "Usage: RingSwarm [options] host port" << std::endl
                  << desc << std::endl;
        return -1;
    }

    if (vm.count("help")) {
        std::cout << "Usage: RingSwarm [options] host port" << std::endl
                  << desc << std::endl;
        return 0;
    }

    boost::log::core::get()->set_filter(
            boost::log::trivial::severity >= boost::log::trivial::trace);

    std::string path = "ring-swarm-" + std::to_string(scenario) + ".sqlite3";
    storage::loadStorage(path.c_str());
    crypto::loadNodeKeys();
    std::filesystem::create_directory("./storage");
    std::string fuseMP = getenv("HOME") + std::string("/fuse") +
std::to_string(scenario); try { std::filesystem::create_directory(fuseMP); }
catch (std::filesystem::filesystem_error &e) { system(("fusermount3 -uz " +
fuseMP).c_str());
    }
//    RingSwarm::fuse::startFuse(fuseMP);

    async::initEventLoop();
    auto *server = new transport::PlainSocketServer(host, port);
    core::Node::thisNode->connectionInfo = server->getConnectionInfo();
    server->listen();

    async::runTaskHandlers(*/
    /*std::max<int>(1, std::thread::hardware_concurrency())*/ /* 1);

    switch (scenario) {
        case 1: {
            //            auto d =
    client::uploadFile("/home/user/Videos/2023-06-07 15-11-20.mp4", 3);
            fuse::mountRing(core::Id::fromHexRepresentation(
                    "e8f0a96cdfdd4bf16e92b1d3b6c306d4e60c013338ea4fead9c7dfa82ae55dfc"));
            break;
        }
        case 2: {
            auto *pubKey = new core::PublicKey();
            boost::algorithm::unhex(remotePubKey, pubKey->begin());
            auto *node = new core::Node(pubKey->getId(), pubKey,
                                        new
    transport::PlainSocketConnectionInfo("localhost", port - 1));
            std::get<0>(core::getOrConnect(node)->await());
            fuse::mountRing(core::Id::fromHexRepresentation(
                    "e8f0a96cdfdd4bf16e92b1d3b6c306d4e60c013338ea4fead9c7dfa82ae55dfc"));
            std::ifstream file("/home/user/Videos/2023-06-07 15-11-20.mp4");
            break;
        }
        default:
            throw core::RingSwarmException();
    }

    std::cin.get();
    storage::closeStorage();
    async::getEventLoop()->stop();
    async::interruptEventLoop();
    fuse::stopFuse();
    BOOST_LOG_TRIVIAL(trace) << "aefesfesfesd";
    exit(0);*/
}
