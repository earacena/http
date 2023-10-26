// C++ standard library
#include <array>
#include <format>
#include <iostream>
#include <vector>

// C standard library
#include <cstdio>
#include <cstdlib>
#include <cstring>

// Unix/Linux
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT "9999"
#define BUFFER_SIZE 256

void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

std::string get_client_address(const struct sockaddr_storage &remote_address,
                               std::array<char, INET6_ADDRSTRLEN> &remote_ip) {
  return std::string(inet_ntop(remote_address.ss_family,
                               get_in_addr((struct sockaddr *)&remote_address),
                               remote_ip.data(), INET6_ADDRSTRLEN));
}

struct addrinfo *get_available_addresses() {
  int rv;
  struct addrinfo *ai, hints;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if ((rv = getaddrinfo(nullptr, PORT, &hints, &ai)) != 0) {
    fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
    exit(1);
  }

  return ai;
}

int bind_available_address(struct addrinfo *ai, int &listener) {
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
}

int get_listener_socket() {
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

int main() {
  int listener;  // Listening socket descriptor
  int newfd;     // Newly accept()ed socket descriptor

  struct sockaddr_storage remote_address;  // Client Address
  socklen_t address_len;

  std::array<char, BUFFER_SIZE> buffer;  // Client data buffer
  std::array<char, INET6_ADDRSTRLEN> remote_ip;
  std::vector<pollfd> pollfds;

  listener = get_listener_socket();

  if (listener == -1) {
    fprintf(stderr, "error getting listening socket\n");
    exit(1);
  }

  // Add listener to fd set
  pollfd pollfd{listener, POLLIN};
  pollfds.push_back(pollfd);

  while (true) {
    int poll_count = poll(pollfds.data(), pollfds.size(), -1);

    if (poll_count == -1) {
      perror("poll");
      exit(1);
    }

    for (int i = 0; i < pollfds.size(); ++i) {
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
                "pollserver: new connection from {} on socket {}\n",
                get_client_address(remote_address, remote_ip), newfd);
          }
        } else {
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
            for (auto &pfd : pollfds) {
              int dest_fd = pfd.fd;

              if (dest_fd != listener && dest_fd != sender_fd) {
                if (send(dest_fd, buffer.data(), nbytes, 0) == -1) {
                  perror("send");
                }
              }
            }
          }
        }
      }
    }
  }

  return 0;
}
