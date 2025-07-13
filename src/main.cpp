//
// shh - C++ WebServer Test
// Simple test main.cpp for the webserver
//

#include "socket.hpp"
#include "epoll.hpp"
#include "request.hpp"
#include "response.hpp"
#include "buffer.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <fstream>

const int PORT = 8080;
const int MAX_EVENTS = 1024;
const int TIMEOUT_MS = 5000;

void check_web_files(const std::string& web_root) {
    // Check if web directory and files exist
    std::ifstream index_check(web_root + "/index.html");
    std::ifstream error404_check(web_root + "/404.html");

    if (!index_check.is_open()) {
        std::cerr << "Warning: " << web_root << "/index.html not found\n";
    } else {
        std::cout << "Found: " << web_root << "/index.html\n";
        index_check.close();
    }

    if (!error404_check.is_open()) {
        std::cerr << "Warning: " << web_root << "/404.html not found\n";
    } else {
        std::cout << "Found: " << web_root << "/404.html\n";
        error404_check.close();
    }
}

void handle_client_request(int client_fd, const std::string& web_root) {
    shh::Buffer buffer(4096);
    shh::HttpRequest request;
    shh::HttpResponse response;

    // Read data from client
    char temp_buffer[4096];
    ssize_t bytes_read = recv(client_fd, temp_buffer, sizeof(temp_buffer) - 1, 0);

    if (bytes_read <= 0) {
        std::cerr << "Failed to read from client or client disconnected\n";
        return;
    }

    temp_buffer[bytes_read] = '\0';
    buffer.BufferAppend(temp_buffer, bytes_read);

    std::cout << "Received request:\n" << temp_buffer << std::endl;

    // Parse the request
    if (!request.parse(buffer)) {
        std::cerr << "Failed to parse HTTP request\n";
        return;
    }

    if (!request.isComplete()) {
        std::cerr << "Incomplete HTTP request\n";
        return;
    }

    // Get request details
    std::string method = request.getMethod();
    std::string path = request.getPath();
    std::string version = request.getVersion();

    std::cout << "Parsed request: " << method << " " << path << " HTTP/" << version << std::endl;

    // Handle root path
    if (path == "/") {
        path = "/index.html";
    }

    // Prepare response
    shh::Buffer response_buffer(4096);
    bool keep_alive = (request.getHeader("connection") == "keep-alive");

    response.init(web_root, path, keep_alive, -1);
    response.MakeResponse(response_buffer);

    // Send response
    const char* response_data = response_buffer.Peek();
    size_t response_size = response_buffer.ReadableBytes();

    ssize_t bytes_sent = send(client_fd, response_data, response_size, 0);
    if (bytes_sent < 0) {
        std::cerr << "Failed to send response\n";
    } else {
        std::cout << "Response sent (" << bytes_sent << " bytes)\n";
    }
}

int main() {
    const std::string web_root = "../public";  // Match your actual directory structure

    try {
        // Check for existing web files
        check_web_files(web_root);
        std::cout << "Using existing HTML files in " << web_root << " directory\n";

        // Create socket
        shh::Socket server_socket(AF_INET, SOCK_STREAM, 0, PORT, INADDR_ANY, 10);
        server_socket.bind_listen();

        int listen_fd = server_socket.get_sock();
        std::cout << "Server listening on port " << PORT << std::endl;

        // Create epoll instance
        shh::Epoll epoll_instance(MAX_EVENTS);

        // Add listening socket to epoll
        epoll_instance.add(listen_fd, EPOLLIN | EPOLLET);

        std::cout << "Server started. Press Ctrl+C to stop.\n";
        std::cout << "Visit http://localhost:" << PORT << " in your browser\n";

        // Main event loop
        while (true) {
            int events_count = epoll_instance.wait(listen_fd, MAX_EVENTS, TIMEOUT_MS);

            if (events_count < 0) {
                std::cerr << "epoll_wait failed\n";
                break;
            }

            if (events_count == 0) {
                // Timeout - continue loop
                continue;
            }

            // Handle events directly since e_handler is incomplete
            for (int i = 0; i < events_count; ++i) {
                int fd = epoll_instance.events[i].data.fd;
                uint32_t events = epoll_instance.events[i].events;

                if (fd == listen_fd) {
                    // New connection - accept all pending connections
                    struct sockaddr_in client_addr;
                    socklen_t client_len = sizeof(client_addr);
                    int client_fd;

                    while ((client_fd = accept(listen_fd,
                                             reinterpret_cast<struct sockaddr*>(&client_addr),
                                             &client_len)) > 0) {

                        std::cout << "New client connected: " << client_fd << std::endl;

                        // Handle the request immediately
                        handle_client_request(client_fd, web_root);

                        // Close the connection
                        close(client_fd);
                    }

                    // Check for errors (but EAGAIN/EWOULDBLOCK is normal for non-blocking)
                    if (client_fd < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
                        std::cerr << "Accept error: " << strerror(errno) << std::endl;
                    }
                } else {
                    // Handle client data (if using persistent connections)
                    if (events & (EPOLLERR | EPOLLHUP)) {
                        std::cerr << "Client error on fd " << fd << std::endl;
                        close(fd);
                    } else if (events & EPOLLIN) {
                        handle_client_request(fd, web_root);
                        close(fd);
                    }
                }
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
