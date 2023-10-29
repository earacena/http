#ifndef SERVER_HPP
#define SERVER_HPP

// C++ headers
#include <array>
#include <format>
#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <algorithm>

// C headers
#include <cstdio>
#include <cstdlib>
#include <cstring>

// Linux/Unix headers
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_SIZE 256

class Server {
 public:
  Server(const int port);
  int initialize();
  void loop();

 private:
  // Data members
  int port = 9999;                       // Port for server to listen on
  int listener;                          // Listening socket descriptor
  std::array<char, BUFFER_SIZE> buffer;  // Buffer for client data
  std::vector<pollfd> pollfds;  // List of active sockets
  std::filesystem::path html_files_path = std::filesystem::current_path().append("examples/pages");

  // Member functions
  int get_listener_socket();
  void *get_in_addr(struct sockaddr *sa);
  std::string get_client_address(const struct sockaddr_storage &remote_address,
                                 std::array<char, INET6_ADDRSTRLEN> &remote_ip);
  struct addrinfo *get_available_addresses();
  int bind_available_address(struct addrinfo *ai, int &listener);
  std::string fetch_file_contents(const std::string & filename);
};

#endif  // SERVER_HPP