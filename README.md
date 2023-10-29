# HTTP Server

An implementation of a non-production HTTP server written in C++20.

## Motivation

I was interested in diving deeper into common web technologies and standards. Therefore I decided to begin implementing an HTTP server starting from the earliest protocol standard, HTTP/0.9, and working my way up to newer standards like HTTP/2.

## Features

* Server (using poll())

HTML Standard Implemented:

* [ ] HTML/0.9
* [ ] HTML/1.0
* [ ] HTML/1.1

## Usage

### Requirements

* Linux (not tested on other platforms)
* CMake 3.17 or better
* A C++20 compatible compiler
* telnet (for sending requests to HTML/0.9)

### Building

To configure:

```bash
cmake -S . -B build
```

To build:

```bash
cmake --build build
```

### Running the server

To run server:

```bash
./build/apps/http
```

### Connecting to the server

#### HTML/0.9

To send a HTML/0.9 request to the server, first build and run the server:

```bash
cmake -S . -B build
  ... generated cmake output ...
cmake --build build
  ... generated cmake output ...
./build/apps/http
```

Open another terminal window and connect using telnet:

```bash
telnet localhost 9999

Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
```

Then type an HTML/0.9 compliant request. Since HTML/0.9 only supports GET, the request should follow the following format:

```text
GET /<HTML filename>
```

Some examples:

```text
GET /
```

```text
GET /index.html
```

The reply from the server should be HTML marked up text from the requested file, or no response if the file is not found:

```html
<html> This is an HTML file! </html>
```

## Disclaimer

This project was made purely for learning purposes and should never be used for production purposes.

## License

MIT License
