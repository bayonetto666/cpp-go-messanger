cmake_minimum_required(VERSION 3.5)
project(auth_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -v")
set(CMAKE_PREFIX_PATH "$MY_INSTALL_DIR")



# Укажите, какие исходники следует включить в проект
set(SOURCE_FILES
    main.cpp
    Auth.cpp
    Hasher.cpp
    DatabaseHandler.pb.cc
    DatabaseHandler.grpc.pb.cc
    AuthServer.grpc.pb.cc
    AuthServer.pb.cc
)


# Добавьте пути к заголовочным файлам и библиотекам
include_directories(
    /opt/homebrew/Cellar/argon2/20190702_1/include
    /Users/kiryl/Documents/cpplibs/jwt-cpp-master/include
    /opt/homebrew/Cellar/openssl@3/3.1.4/include
    /opt/homebrew/Cellar/nlohmann-json/3.11.2/include
    /Users/kiryl/.local/include

)

link_directories(
    /opt/homebrew/Cellar/openssl@3/3.1.4/lib
    /Users/kiryl/.local/lib
    /opt/homebrew/Cellar/argon2/20190702_1/lib
)

add_executable(auth_server ${SOURCE_FILES})

find_library(CORE_FOUNDATION CoreFoundation)
find_library(LIBCARES cares)

# Link libraries to the target
target_link_libraries(auth_server
    argon2.1
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