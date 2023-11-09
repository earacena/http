#include "server.hpp"

int Server::initialize() {
  listener = get_listener_socket();

  if (listener == -1) {
    std::cerr << "error getting listening socket\n";
    return -1;
  }

  // Keep track of listener fd
  pollfd pfd{listener, POLLIN};
  pollfds.push_back(pfd);

  return 0;
}

void Server::loop() {

  int newfd, poll_count;
  sockaddr_storage remote_address;
  socklen_t address_len;
  std::array<char, INET6_ADDRSTRLEN> remote_ip;

  while (true) {
    poll_count = poll(pollfds.data(), pollfds.size(), -1);

    if (poll_count == -1) {
      perror("poll");
      exit(1);
    }

    // Iterate in reverse to allow for deletion while looping
    for (int i = pollfds.size() - 1; i >= 0; --i) {
      struct pollfd &pfd = pollfds.at(i);

      if (pfd.revents & POLLIN) {
        if (pfd.fd == listener) {
          // listener is ready, new connection is handled

          address_len = sizeof remote_address;
          newfd = accept(listener, (struct sockaddr *)&remote_address,
                         &address_len);

          if (newfd == -1) {
            perror("accept");
          } else {
            struct pollfd new_pollfd {
              newfd, POLLIN
            };

            pollfds.push_back(new_pollfd);

            std::cout << std::format(
                "[server] new connection from {} on socket {}\n",
                get_client_address(remote_address, remote_ip), newfd);
          }
        } else {
          // Data received from connection
          int nbytes = recv(pfd.fd, buffer.data(), sizeof buffer, 0);
          int sender_fd = pfd.fd;


          if (nbytes <= 0) {
            if (nbytes == 0) {
              printf("pollserver: socket %d hung up\n", sender_fd);
            } else {
              perror("recv");
            }

            close(pfd.fd);
            pollfds.erase(pollfds.begin() + i);
          } else {
            std::string request = std::string(buffer.data(), nbytes);
            const std::string response = http_protocol->respond(request);

            if (response != "") {
                if (send(sender_fd, response.data(), response.size(), 0) == -1) {
                  perror("send");
                }

                std::cout << std::format(" - found, {} byte response\n", response.size());
            } else {
                std::cout << std::format(" - no response\n");
            }

            // Terminate connection
            close(pfd.fd);
            pollfds.erase(pollfds.begin() + i);
          }
        }
      }
    }
  }
}

void *Server::get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

std::string Server::get_client_address(
    const struct sockaddr_storage &remote_address,
    std::array<char, INET6_ADDRSTRLEN> &remote_ip) {
  return std::string(inet_ntop(remote_address.ss_family,
                               get_in_addr((struct sockaddr *)&remote_address),
                               remote_ip.data(), INET6_ADDRSTRLEN));
}

struct addrinfo *Server::get_available_addresses() {
  int rv;
  struct addrinfo *ai, hints;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if ((rv = getaddrinfo(nullptr, std::to_string(port).c_str(), &hints, &ai)) != 0) {
    fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
    exit(1);
  }

  return ai;
}

int Server::bind_available_address(struct addrinfo *ai, int &listener) {
  int yes = 1;

  struct addrinfo *p;
  for (p = ai; p != NULL; p = p->ai_next) {
    listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

    if (listener < 0) {
      continue;
    }

    // Silence "address already in use" error message
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
      close(listener);
      continue;
    }

    break;
  }

  freeaddrinfo(ai);

  if (p == nullptr) {
    return -1;
  }

  return 0;
}

std::string Server::fetch_file_contents(const std::string &resource_path) {
  std::ifstream file(html_files_path.string() + resource_path);

  if (!file.is_open()) {
    std::cerr << std::format("Could not open file '{}'\n", html_files_path.string() + resource_path);
    return "";
  }

  std::string line;
  std::string data;
  while (std::getline(file, line)) {
    data.append(line);
  }

  data.append("\n");

  return data;
}

int Server::get_listener_socket() {
  int listener;

  struct addrinfo *ai = get_available_addresses();

  if (bind_available_address(ai, listener) == -1) {
    return -1;
  }

  if (listen(listener, 10) == -1) {
    return -1;
  }

  return listener;
}