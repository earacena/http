add_executable(http http.cpp)
target_compile_features(http PRIVATE cxx_std_20)

# Link libraries
target_link_libraries(http PRIVATE server)