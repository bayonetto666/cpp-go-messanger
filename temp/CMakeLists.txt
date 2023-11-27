cmake_minimum_required(VERSION 3.5)
project(server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

# External libraries paths
set(ARGON2_PATH "")
set(JWT_MASTER_PATH "")

set(gRPC_INCLUDE_PATH "")
set(PROTOBUF_INCLUDE_PATH "")
set(ABSL_INCLUDE_PATH "")
set(RE2_INCLUDE_PATH "")
set(UTF8_INCLUDE_PATH "")

set(gRPC_LIB_PATH "")
set(PROTOBUF_LIB_PATH "")
set(ABSL_LIB_PATH "")
set(RE2_LIB_PATH "")
set(UTF8_LIB_PATH "")

# Include and link directories
include_directories(
  ${gRPC_INCLUDE_PATH}
  ${PROTOBUF_INCLUDE_PATH}
  ${ABSL_INCLUDE_PATH}
  ${RE2_INLCLUDE_ATH}
  ${UTF8_INCLUDE_PATH}
  common/
  ${ARGON2_PATH}/include
  ${JWT_MASTER_PATH}/include
)

link_directories(
  ${gRPC_LIB_PATH}
  ${PROTOBUF_LIB_PATH}
  ${ABSL_LIB_PATH}
  ${RE2_LIB_PATH}
  ${UTF8_LIB_PATH}
)

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
  absl_bad_any_cast_impl
  absl_bad_optional_access      
  absl_bad_variant_access      
  absl_base      
  absl_city      
  absl_civil_time      
  absl_cord_internal      
  absl_cord      
  absl_cordz_functions      
  absl_cordz_handle     
  absl_cordz_info     
  absl_cordz_sample_token      
  absl_crc_cord_state      
  absl_crc_cpu_detect      
  absl_crc_internal      
  absl_crc32c      
  absl_debugging_internal      
  absl_demangle_internal      
  absl_die_if_null      
  absl_examine_stack      
  absl_exponential_biased      
  absl_failure_signal_handler      
  absl_flags_commandlineflag_internal      
  absl_flags_commandlineflag      
  absl_flags_config      
  absl_flags_internal      
  absl_flags_marshalling      
  absl_flags_parse      
  absl_flags_private_handle_accessor      
  absl_flags_program_name      
  absl_flags_reflection      
  absl_flags_usage_internal
  absl_flags_usage     
  absl_flags      
  absl_graphcycles_internal      
  absl_hash      
  absl_hashtablez_sampler      
  absl_int128      
  absl_kernel_timeout_internal      
  absl_leak_check      
  absl_log_entry      
  absl_log_flags      
  absl_log_globals      
  absl_log_initialize      
  absl_log_internal_check_op      
  absl_log_internal_conditions      
  absl_log_internal_format      
  absl_log_internal_globals      
  absl_log_internal_log_sink_set      
  absl_log_internal_message      
  absl_log_internal_nullguard      
  absl_log_internal_proto      
  absl_log_severity      
  absl_log_sink      
  absl_low_level_hash      
  absl_malloc_internal      
  absl_periodic_sampler      
  absl_random_distributions      
  absl_random_internal_distribution_test_util      
  absl_random_internal_platform      
  absl_random_internal_pool_urbg      
  absl_random_internal_randen_hwaes_impl      
  absl_random_internal_randen_hwaes      
  absl_random_internal_randen_slow      
  absl_random_internal_randen      
  absl_random_internal_seed_material      
  absl_random_seed_gen_exception      
  absl_random_seed_sequences      
  absl_raw_hash_set      
  absl_raw_logging_internal      
  absl_scoped_set_env      
  absl_spinlock_wait      
  absl_stacktrace      
  absl_status     
  absl_statusor      
  absl_str_format_internal      
  absl_strerror      
  absl_string_view      
  absl_strings_internal      
  absl_strings      
  absl_symbolize      
  absl_synchronization      
  absl_throw_delegate      
  absl_time_zone      
  absl_time

  address_sorting      
  cares      
  crypto      
  gpr      
  grpc_authorization_provider      
  grpc_plugin_support      
  grpc_unsecure      
  grpc      
  grpc++_alts      
  grpc++_error_details      
  grpc++_reflection      
  grpc++_unsecure      
  grpc++      
  grpcpp_channelz      
  
  protobuf-lite      
  protobuf      
  protoc      
  
  re2      
  ssl      
  
  upb_collections_lib      
  upb_json_lib        
  upb_textformat_lib       
  upb      
  
  utf8_range_lib        
  utf8_range      
  utf8_validity    
  
  z    
)

target_link_libraries(api_gateway
  PRIVATE AuthenticationLib
  PRIVATE DatabaseLib
  PRIVATE ChatLib
  ${COMMON_LIBRARIES}
  ${Boost_LIBRARIES}
  ${EXTERNAL_LIBRARIES}
)

target_link_libraries(database_handler
  ${COMMON_LIBRARIES}
  ${EXTERNAL_LIBRARIES}
  mongo::mongocxx_shared
  mongo::bsoncxx_shared
)

target_link_libraries(auth_server
  ${COMMON_LIBRARIES}
  ${ARGON2_PATH}/lib/libargon2.a
  ${EXTERNAL_LIBRARIES}
)
