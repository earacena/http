#include "protocol.hpp"

std::string HttpZeroProtocol::respond(const std::string & request) {
  // Extract method and requested resource
  std::string method;
  std::string resource_path;
  std::stringstream sstream(request);
  sstream >> method >> resource_path;

  std::string response = "";

  if (method == "GET") {    
    // Find and serve requested file if it exists
    if (resource_path == "/") {
      resource_path = "/index.html";
    }

    std::cout << std::format("[{}] {}", method, resource_path);
    std::filesystem::path html_files_path = std::filesystem::current_path().append("examples/pages");
    response = fetch_file_contents(html_files_path, resource_path);
  }

  return response;
}

std::string HttpProtocol::fetch_file_contents(const std::filesystem::path & html_files_path, std::string &resource_path) {
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

std::unique_ptr<HttpProtocol> HttpProtocolCreator::create(HttpStandard standard) {
  if (standard == HttpStandard::HTTP_0_9) {
    return std::make_unique<HttpZeroProtocol>();
  }

  return nullptr;
}
