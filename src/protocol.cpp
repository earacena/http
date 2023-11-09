#include "protocol.hpp"

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

std::string HttpZeroProtocol::respond(const std::string &request) {
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

std::string HttpOneProtocol::respond(const std::string &request) {
  // Parse request by lines
  std::vector<std::string> request_lines;
  std::stringstream request_stream(request);
  for (std::string line; std::getline(request_stream, line); ) {
    request_lines.push_back(line);
  }

  // Determine if request is a simple request or full request
  bool is_simple_request = request_lines.size() == 1;

  // Extract method and requested resource
  std::string response = "";
  if (is_simple_request) {
    // Process a simple request which follows HTTP/0.9 format
    // format: method_token resource_uri
    // example: GET /index.html
    std::string method;
    std::string resource_path;
    std::stringstream sstream(request);
    sstream >> method >> resource_path;

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

  } else {
    // Process a full request
    // format:
    //  Request-line:                   GET /index.html HTML/1.0
    //  General Header:                 Date: Tue, 15 Nov 1994 08:12:31 GMT
    //                                  Pragma
    //  Request Header (any or none):   Authorization        
    //                                  From             
    //                                  If-Modified-Since
    //                                  Referer          
    //                                  User-Agent  
    //   Entity Header:                 Allow           
    //                                  Content-Encoding
    //                                  Content-Length  
    //                                  Content-Type    
    //                                  Expires         
    //                                  Last-Modified   
    //                                  extension-header
    //  CLRF (newline)
    //  Entity Body (optional)        Octet
  //}

  // std::vector<std::string> request_headers = {
  //   "Authorization",
  //   "From",
  //   "If-Modified-Since",
  //   "Referer",
  //   "User-Agent"
  // };

  // std::vector<std::string> entity_headers = {
  //   "Allow",
  //   "Content-Encoding",
  //   "Content-Length",
  //   "Content-Type",
  //   "Expires",
  //   "Last-Modified",
  //   "extention-header"
  // };

  // std::string parsed_request_line;
  // std::string parsed_general_header;
  // std::string parsed_request_header;
  // std::string parsed_entity_header;

  // for (const std::string & line : request_lines) {
    return response;
  }

}

std::unique_ptr<HttpProtocol> HttpProtocolCreator::create(HttpStandard standard) {
  if (standard == HttpStandard::HTTP_0_9) {
    return std::make_unique<HttpZeroProtocol>();
  }

  return nullptr;
}
