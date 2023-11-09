add_library(server server.cpp server.hpp)
add_library(protocol protocol.cpp protocol.hpp)
target_include_directories(server PUBLIC .)
target_compile_features(server PUBLIC cxx_std_20)