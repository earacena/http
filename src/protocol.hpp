#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include <format>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

enum HttpStandard {
  HTTP_0_9,
};

// HTTP Protocol interface
class HttpProtocol {
public:
  virtual std::string respond(const std::string &request) = 0;
  virtual ~HttpProtocol() = default;
  std::string fetch_file_contents(const std::filesystem::path & html_files_path, std::string &resource_path);
};

// HTTP/0.9 protocol implementation
class HttpZeroProtocol : public HttpProtocol {
public:
  std::string respond(const std::string &request) override;
};


// HTTP/1.0 protocol implementation
class HttpOneProtocol : public HttpProtocol {
public:
  std::string respond(const std::string &request) override;
};

// HTTP Protocol Factory
class HttpProtocolCreator {
public:
  virtual std::unique_ptr<HttpProtocol> create(HttpStandard standard);
  virtual ~HttpProtocolCreator() = default;
};

#endif  // PROTOCOL_HPP