add_library(server server.cpp server.hpp)
target_include_directories(server PUBLIC .)
target_compile_features(server PUBLIC cxx_std_20)