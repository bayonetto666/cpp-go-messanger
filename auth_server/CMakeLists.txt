cmake_minimum_required(VERSION 3.5)
project(AuthServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -v")


# Укажите, какие исходники следует включить в проект
set(SOURCE_FILES
    main.cpp
    Auth.cpp
    Hasher.cpp
    DatabaseHandler.pb.cc
    DatabaseHandler.grpc.pb.cc
)


# Добавьте пути к заголовочным файлам и библиотекам
include_directories(
    /opt/homebrew/Cellar/argon2/20190702_1/include
    /Users/kiryl/Documents/cpplibs/jwt-cpp-master/include
    /opt/homebrew/Cellar/openssl@3/3.1.3/include
    /opt/homebrew/Cellar/nlohmann-json/3.11.2/include
    /opt/homebrew/Cellar/mongo-cxx-driver/3.8.1/include/bsoncxx/v_noabi
    /opt/homebrew/Cellar/mongo-cxx-driver/3.8.1/include/mongocxx/v_noabi
    /opt/homebrew/Cellar/protobuf/24.4/include
    /opt/homebrew/Cellar/abseil/20230802.1/include
    /opt/homebrew/Cellar/grpc/1.59.1_1/include
)

link_directories(
    /opt/homebrew/Cellar/openssl@3/3.1.3/lib
    /opt/homebrew/Cellar/mongo-cxx-driver/3.8.1/lib
    /opt/homebrew/Cellar/abseil/20230802.1/lib
    /opt/homebrew/Cellar/protobuf/24.4/lib
    /opt/homebrew/Cellar/grpc/1.59.1_1/lib
    /opt/homebrew/Cellar/argon2/20190702_1/lib
)

add_executable(AuthServer ${SOURCE_FILES})

# Link libraries to the target
target_link_libraries(AuthServer
    argon2.1
    address_sorting.36.0.0
    address_sorting.36
    gpr.36.0.0
    gpr.36
    grpc_authorization_provider.1.59.1
    grpc_authorization_provider.1.59
    grpc_plugin_support.1.59.1
    grpc_plugin_support.1.59
    grpc_unsecure.36.0.0
    grpc_unsecure.36
    grpc.36.0.0
    grpc.36
    grpc++_alts.1.59.1
    grpc++_alts.1.59
    grpc++_error_details.1.59.1
    grpc++_error_details.1.59
    grpc++_reflection.1.59.1
    grpc++_reflection.1.59
    grpc++_test_config.1.59.1
    grpc++_test_config.1.59
    grpc++_unsecure.1.59.1
    grpc++_unsecure.1.59
    grpc++.1.59.1
    grpc++.1.59
    grpcpp_channelz.1.59.1
    grpcpp_channelz.1.59
    upb_collections_lib.36.0.0
    upb_collections_lib.36
    upb_json_lib.36.0.0
    upb_json_lib.36
    upb_textformat_lib.36.0.0
    upb_textformat_lib.36
    upb.36.0.0
    upb.36
    utf8_range_lib.36.0.0
    utf8_range_lib.36
    crypto
    ssl
    bsoncxx
    mongocxx
    protobuf
    protoc
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
    absl_scoped_mock_log
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
)

