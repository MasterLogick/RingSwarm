cmake_minimum_required(VERSION 3.24)
project(RingSwarm)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenSSL REQUIRED COMPONENTS Crypto)
find_library(gmp NAMES gmp)
find_library(gmpxx NAMES gmpxx)
find_package(Boost REQUIRED system log)

add_executable(RingSwarm src/main.cpp src/core/FileMeta.cpp src/core/FileMeta.h src/core/Id.cpp src/core/Id.h src/core/ChunkLink.cpp src/core/ChunkLink.h src/transport/Buffer.cpp src/transport/Buffer.h src/transport/Transport.cpp src/transport/Transport.h src/proto/ServerHandler.cpp src/proto/ServerHandler.h src/proto/handler/HandshakeHandler.cpp src/proto/ProtocolException.h src/transport/PlainSocketTransport.cpp src/transport/PlainSocketTransport.h src/proto/handler/GetChunkLinkHandler.cpp src/storage/ChunkSwarmStorage.cpp src/storage/ChunkSwarmStorage.h src/core/ChunkSwarm.cpp src/core/ChunkSwarm.h src/storage/FileSwarmStorage.cpp src/storage/FileSwarmStorage.h src/core/FileMetaSwarm.cpp src/core/FileMetaSwarm.h src/proto/handler/GetFileMetaHandler.cpp src/storage/ConnectionsStorage.cpp src/storage/ConnectionsStorage.h src/core/Node.cpp src/core/Node.h src/proto/handler/GetNearestChunkHandler.cpp src/core/Random.cpp src/core/Random.h src/proto/handler/GetChunkHandler.cpp src/proto/handler/DragIntoFileMetaSwarmHandler.cpp src/proto/handler/NoticeJoinedFileMetaSwarmHandler.cpp src/core/Swarm.cpp src/core/Swarm.h src/proto/handler/GetFileMetaSwarmHandler.cpp src/proto/handler/GetChunkSwarmHandler.cpp src/proto/handler/NoticeJoinedChunkSwarmHandler.cpp src/proto/handler/NoticeLeavedChunkSwarmHandler.cpp src/proto/handler/SubscribeOnChunkChangeHandler.cpp src/proto/handler/ChunkChangeEventHandler.cpp src/proto/handler/UnsubscribeOnChunkChangeHandler.cpp src/transport/PlainSocketServer.cpp src/transport/PlainSocketServer.h src/proto/ClientHandler.cpp src/proto/ClientHandler.h src/transport/ResponseBuffer.h src/transport/RequestBuffer.h src/proto/TooLargeMessageException.h src/core/RingSwarmException.h src/transport/TransportBackendException.h src/transport/DataSerialisationException.h src/proto/ResponseHeader.h src/transport/ResponseBuffer.cpp src/proto/RequestHeader.h)
#add_executable(RingSwarm src/main.cpp src/swarm/Cache.cpp src/swarm/Cache.h src/entity/Id.cpp src/entity/Id.h src/swarm/KnownNodesStorage.cpp src/swarm/KnownNodesStorage.h src/entity/Node.cpp src/entity/Node.h src/swarm/ConnectionManager.cpp src/swarm/ConnectionManager.h src/crypto/Signer.cpp src/crypto/Signer.h src/swarm/NodeConnection.cpp src/swarm/NodeConnection.h src/swarm/ChunkSwarm.cpp src/swarm/ChunkSwarm.h src/swarm/MetaSwarm.cpp src/swarm/MetaSwarm.h src/client/ChunkSwarmClient.cpp src/client/ChunkSwarmClient.h src/swarm/MetaNode.cpp src/swarm/MetaNode.h src/entity/SignedMeta.cpp src/entity/SignedMeta.h src/swarm/Swarm.cpp src/swarm/Swarm.h src/client/MetaSwarmClient.cpp src/client/MetaSwarmClient.h src/swarm/ChunkNode.cpp src/swarm/ChunkNode.h src/entity/SignedChunk.cpp src/entity/SignedChunk.h src/entity/SignedStaticRingLinks.cpp src/entity/SignedStaticRingLinks.h src/client/RingSwarmClient.cpp src/client/RingSwarmClient.h)
#add_executable(RingSwarm src/main.cpp) # src/swarm/Cache.cpp src/swarm/Cache.h src/entity/Id.cpp src/entity/Id.h src/swarm/KnownNodesStorage.cpp src/swarm/KnownNodesStorage.h src/entity/Node.cpp src/entity/Node.h src/swarm/ConnectionManager.cpp src/swarm/ConnectionManager.h src/crypto/Signer.cpp src/crypto/Signer.h src/swarm/NodeConnection.cpp src/swarm/NodeConnection.h src/swarm/ChunkSwarm.cpp src/swarm/ChunkSwarm.h src/swarm/MetaSwarm.cpp src/swarm/MetaSwarm.h src/client/ChunkSwarmClient.cpp src/client/ChunkSwarmClient.h src/swarm/MetaNode.cpp src/swarm/MetaNode.h src/entity/SignedMeta.cpp src/entity/SignedMeta.h src/swarm/Swarm.cpp src/swarm/Swarm.h src/client/MetaSwarmClient.cpp src/client/MetaSwarmClient.h src/swarm/ChunkNode.cpp src/swarm/ChunkNode.h src/entity/SignedChunk.cpp src/entity/SignedChunk.h src/entity/SignedStaticRingLinks.cpp src/entity/SignedStaticRingLinks.h src/client/RingSwarmClient.cpp src/client/RingSwarmClient.h)
#[[
function(gen_proto proto)
    get_filename_component(abs_proto "src/proto/${proto}.proto" ABSOLUTE)
    get_filename_component(abs_google_protobuff "grpc/third_party/protobuf/src" ABSOLUTE)
    get_filename_component(proto_path "${abs_proto}" PATH)

    add_custom_command(
            OUTPUT
            "${CMAKE_CURRENT_BINARY_DIR}/gen/proto/${proto}.pb.cc"
            "${CMAKE_CURRENT_BINARY_DIR}/gen/proto/${proto}.pb.h"
            "${CMAKE_CURRENT_BINARY_DIR}/gen/proto/${proto}.grpc.pb.cc"
            "${CMAKE_CURRENT_BINARY_DIR}/gen/proto/${proto}.grpc.pb.h"
            COMMAND $<TARGET_FILE:protoc>
            ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}/gen/proto/"
            --cpp_out "${CMAKE_CURRENT_BINARY_DIR}/gen/proto/"
            -I "${proto_path}"
            -I "${abs_google_protobuff}"
            "--plugin=protoc-gen-grpc=$<TARGET_FILE:grpc_cpp_plugin>"
            "${abs_proto}"
            DEPENDS "${abs_proto}")

    add_library("grpc_RingSwarm_${proto}" "${CMAKE_CURRENT_BINARY_DIR}/gen/proto/${proto}.pb.cc" "${CMAKE_CURRENT_BINARY_DIR}/gen/proto/${proto}.grpc.pb.cc")
    target_link_libraries("grpc_RingSwarm_${proto}" PUBLIC grpc++_reflection grpc++ libprotobuf)
    target_link_libraries(RingSwarm PUBLIC "grpc_RingSwarm_${proto}")
endfunction()
]]

#gen_proto(Common)
#gen_proto(ChunkSwarmService)
#gen_proto(MetaSwarmService)
#gen_proto(RingSwarmService)
#target_include_directories(RingSwarm PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/gen/" grpc/third_party/protobuf/src "${OPENSSL_INCLUDE_DIR}" i2pd)
target_include_directories(RingSwarm PUBLIC "${OPENSSL_INCLUDE_DIR}")
target_link_libraries(RingSwarm PUBLIC OpenSSL::Crypto gmp gmpxx ${Boost_LIBRARIES})