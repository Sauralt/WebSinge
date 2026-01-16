*This project has been created as part of the 42 curriculum by mgarsaul and cfleuret.*

## Description

**webserv** is a system programming project from the 42 curriculum that consists of creating a simple HTTP server in **C++**.

The goal of the project is to understand how web servers work internally by reimplementing core features of a real server (such as Nginx or Apache) while strictly following the HTTP/1.1 standard. The server must be capable of handling multiple clients simultaneously using non-blocking I/O, without relying on external libraries.

Key concepts explored in this project include:

* TCP/IP networking
* HTTP/1.1 protocol
* Non-blocking sockets
* I/O multiplexing (`poll`)
* Process and resource management
* Configuration file parsing

## Instructions

### Compilation

Clone the repository and compile the project using `make`:

git clone <repository_url> webserv
cd webserv
make

Executable's name : ./webserv

### Execution

Run the server by providing a configuration file:

./webserv <config_file>

Example:

./webserv config/Singe.conf

This webserv also has a base configuration file and can be executed just as ./webserv

Once running, the server will listen on the ports defined in the configuration file and handle HTTP requests accordingly.

### Notes

* The project is developed and tested on **Linux**.
* Only C++ standard library and system calls are allowed.
* The server must never block and must handle multiple clients concurrently.

## Resources

### Technical References

* RFC 7230–7235 — HTTP/1.1 specifications
* Linux manual pages (`man socket`, `man poll`, `man select`, `man epoll`)
* Beej’s Guide to Network Programming
* Nginx documentation (for behavior reference only)
* [https://man7.org](https://man7.org)
* Building a simple server with C++ :
  https://ncona.com/2019/04/building-a-simple-server-with-cpp/
* HTTP messages from MDN :
  https://developer.mozilla.org/en-US/docs/Web/HTTP/Guides/Messages
* Writing an Nginx-like web server from scratch in C++ from Ali Naqvi's Blog :
  https://www.alimnaqvi.com/blog/webserv#overview-of-http


### Use of Artificial Intelligence

AI tools were used strictly as an **assistance resource** during development, in compliance with 42 rules. Specifically, AI was used to:

* Clarify networking and HTTP protocol concepts
* Help interpret RFC documentation
* Assist with debugging and understanding compiler/runtime errors
* Review code logic and suggest possible optimizations

All architectural decisions, code implementation, testing, and final validation were performed by the project authors. No code was copied directly from AI-generated outputs.

## Additional Information

For detailed requirements, expected behaviors, and constraints, please refer to the official **webserv** subject provided by 42.
