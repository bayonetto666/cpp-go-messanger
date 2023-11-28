cmake_minimum_required(VERSION 3.15)
project(server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

# External libraries paths
set(ARGON2_PATH "path/to/argon2")
set(JWT_CPP_PATH "path/to/jwr-cpp")

# Common source files
set(COMMON_SOURCE_FILES
  common/DBClient.cpp
  common/AuthClient.cpp
  common/ChatClient.cpp
  common/DatabaseHandler.pb.cc
  common/DatabaseHandler.grpc.pb.cc
  common/AuthServer.grpc.pb.cc
  common/AuthServer.pb.cc
  common/ChatServer.pb.cc
  common/ChatServer.grpc.pb.cc
)

# Source files for each executable
set(SOURCE_FILES_API_GATEWAY
  api_gateway/main.cpp
  api_gateway/Server.cpp
  api_gateway/websocket_proxy.cpp
)

set(SOURCE_FILES_DATABASE_SERVER
  database_server/main.cpp
  database_server/DatabaseHandler.cpp
)

set(SOURCE_FILES_AUTH_SERVER
  auth_server/main.cpp
  auth_server/Auth.cpp
  auth_server/Hasher.cpp
)

# Create executables
add_executable(api_gateway ${SOURCE_FILES_API_GATEWAY} ${COMMON_SOURCE_FILES})
add_executable(database_handler ${SOURCE_FILES_DATABASE_SERVER} ${COMMON_SOURCE_FILES})
add_executable(auth_server ${SOURCE_FILES_AUTH_SERVER} ${COMMON_SOURCE_FILES})

# Find packages
find_package(nlohmann_json 3.11.2 REQUIRED)
find_package(mongocxx REQUIRED)
find_package(bsoncxx REQUIRED)
find_package(Boost 1.82.0 REQUIRED COMPONENTS system thread)
find_package(OpenSSL REQUIRED)

# find_package(gRPC CONFIG REQUIRED)
# find_package(absl CONFIG REQUIRED)
# find_package(utf8_range CONFIG REQUIRED)
# find_package(Protobuf CONFIG REQUIRED)


# Include directories
include_directories(
  common/
  ${ARGON2_PATH}/include
  ${JWT_CPP_PATH}/include
  
)

# Create libraries
add_library(AuthenticationLib STATIC ${COMMON_SOURCE_FILES})
target_include_directories(AuthenticationLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(DatabaseLib STATIC ${COMMON_SOURCE_FILES})
target_include_directories(DatabaseLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(ChatLib STATIC ${COMMON_SOURCE_FILES})
target_include_directories(ChatLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Linking libraries
set(COMMON_LIBRARIES
  nlohmann_json::nlohmann_json
  ${CORE_FOUNDATION}
  ${LIBCARES}
  OpenSSL::SSL
)

set(EXTERNAL_LIBRARIES
  absl::absl_check
  absl::absl_log 
  absl::algorithm 
  absl::base 
  absl::bind_front
  absl::bits 
  absl::btree 
  absl::cleanup 
  absl::cord 
  absl::core_headers
  absl::debugging
  absl::die_if_null 
  absl::dynamic_annotations 
  absl::flags
  absl::flat_hash_map 
  absl::flat_hash_set 
  absl::function_ref 
  absl::hash
  absl::layout 
  absl::log_initialize 
  absl::log_severity 
  absl::memory
  absl::node_hash_map 
  absl::node_hash_set 
  absl::optional 
  absl::span
  absl::status 
  absl::statusor 
  absl::strings 
  absl::synchronization
  absl::time
  absl::type_traits 
  absl::utility 
  absl::variant 
  utf8_range::utf8_validity
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF}
)

target_link_libraries(api_gateway
PRIVATE
  ${EXTERNAL_LIBRARIES}
  AuthenticationLib
  DatabaseLib
  ChatLib
  ${COMMON_LIBRARIES}
  ${Boost_LIBRARIES}
)

target_link_libraries(database_handler
PRIVATE
  ${EXTERNAL_LIBRARIES}
  ${COMMON_LIBRARIES}
  mongo::mongocxx_shared
  mongo::bsoncxx_shared
  
)

target_link_libraries(auth_server
PRIVATE
  ${EXTERNAL_LIBRARIES}
  ${COMMON_LIBRARIES}
  ${ARGON2_PATH}/lib/libargon2.a
  
)
