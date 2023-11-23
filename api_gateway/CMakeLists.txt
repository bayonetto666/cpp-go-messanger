cmake_minimum_required(VERSION 3.5)
project(api_gateway)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -v")

# Добавьте пути к заголовочным файлам и библиотекам
include_directories(
  /Users/kiryl/.local/include
  /Users/kiryl/.local/include/google
  ${CMAKE_CURRENT_SOURCE_DIR}/../common
)

# Укажите, какие исходники следует включить в проект
set(SOURCE_FILES
  main.cpp
  Server.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../common/DBClient.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../common/AuthClient.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../common/ChatClient.cpp
  websocket_proxy.cpp 
)

link_directories(
  /Users/kiryl/.local/lib
)

add_executable(api_gateway ${SOURCE_FILES})

find_library(CORE_FOUNDATION CoreFoundation)
find_library(LIBCARES cares)

find_package(Boost 1.82.0 REQUIRED COMPONENTS system thread)
include_directories(${Boost_INCLUDE_DIRS})

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

find_package(nlohmann_json REQUIRED)
include_directories(${nlohmann_json_INCLUDE_DIRS})


add_library(AuthenticationLib STATIC 
  ${CMAKE_CURRENT_SOURCE_DIR}/../common/AuthServer.pb.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/../common/AuthServer.grpc.pb.cc
)
target_include_directories(AuthenticationLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})  # Добавьте эту строку

add_library(DatabaseLib STATIC 
  ${CMAKE_CURRENT_SOURCE_DIR}/../common/DatabaseHandler.pb.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/../common/DatabaseHandler.grpc.pb.cc
)

target_include_directories(DatabaseLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})  # Добавьте эту строку

add_library(ChatLib STATIC
  ${CMAKE_CURRENT_SOURCE_DIR}/../common/ChatServer.pb.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/../common/ChatServer.grpc.pb.cc
)

target_include_directories(ChatLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})  # Добавьте эту строку

# Link libraries to the target
target_link_libraries(api_gateway
    PRIVATE AuthenticationLib
    PRIVATE DatabaseLib
    PRIVATE ChatLib
    ${OPENSSL_LIBRARIES}
    ${Boost_LIBRARIES}
    ${CORE_FOUNDATION}
    ${LIBCARES}
    -lz
    ssl
    crypto
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
    absl_flags_usage_internal      absl_flags_usage      absl_flags      absl_graphcycles_internal      absl_hash      absl_hashtablez_sampler      absl_int128      absl_kernel_timeout_internal      absl_leak_check      absl_log_entry      absl_log_flags      absl_log_globals      absl_log_initialize      absl_log_internal_check_op      absl_log_internal_conditions      absl_log_internal_format      absl_log_internal_globals      absl_log_internal_log_sink_set      absl_log_internal_message      absl_log_internal_nullguard      absl_log_internal_proto      absl_log_severity      absl_log_sink      absl_low_level_hash      absl_malloc_internal      absl_periodic_sampler      absl_random_distributions      absl_random_internal_distribution_test_util      absl_random_internal_platform      absl_random_internal_pool_urbg      absl_random_internal_randen_hwaes_impl      absl_random_internal_randen_hwaes      absl_random_internal_randen_slow      absl_random_internal_randen      absl_random_internal_seed_material      absl_random_seed_gen_exception      absl_random_seed_sequences      absl_raw_hash_set      absl_raw_logging_internal      absl_scoped_set_env      absl_spinlock_wait      absl_stacktrace      absl_status      absl_statusor      absl_str_format_internal      absl_strerror      absl_string_view      absl_strings_internal      absl_strings      absl_symbolize      absl_synchronization      absl_throw_delegate      absl_time_zone      absl_time      address_sorting      cares      crypto      gpr      grpc_authorization_provider      grpc_plugin_support      grpc_unsecure      grpc      grpc++_alts      grpc++_error_details      grpc++_reflection      grpc++_unsecure      grpc++      grpcpp_channelz      protobuf-lite      protobuf      protoc      re2      ssl      upb_collections_lib      upb_json_lib        upb_textformat_lib       upb      utf8_range_lib        utf8_range      utf8_validity    z    libz.1.dylib

)