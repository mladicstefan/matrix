//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#include "socket.hpp"
#include "epoll.hpp"
#include <iostream>
#include <sys/epoll.h>
#include <chrono>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "buffer.hpp"
#include "request.hpp"

void test_epoll_creation() {
    std::cout << "=== Testing Epoll Creation ===" << '\n';
    try {
        shh::Epoll epoll(10);
        std::cout << "✓ Epoll created successfully with maxEvents=10" << '\n';
    } catch (const std::exception& e) {
        std::cerr << "✗ Epoll creation failed: " << e.what() << '\n';
    }
}

void test_epoll_add_remove() {
    std::cout << "\n=== Testing Epoll Add/Remove ===" << '\n';
    try {
        shh::Epoll epoll(10);

        // Create a test socket
        int test_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (test_fd < 0) {
            throw std::runtime_error("Failed to create test socket");
        }

        std::cout << "Test socket FD: " << test_fd << '\n';

        // Test add
        epoll.add(test_fd, EPOLLIN | EPOLLET);
        std::cout << "✓ Added fd to epoll successfully" << '\n';

        // Test modify
        epoll.mod(test_fd, EPOLLOUT | EPOLLET);
        std::cout << "✓ Modified fd in epoll successfully" << '\n';

        // Test delete
        epoll.del(test_fd, EPOLLOUT);
        std::cout << "✓ Deleted fd from epoll successfully" << '\n';

        close(test_fd);

    } catch (const std::exception& e) {
        std::cerr << "✗ Epoll add/remove test failed: " << e.what() << '\n';
    }
}

void test_epoll_wait_timeout() {
    std::cout << "\n=== Testing Epoll Wait with Timeout ===" << '\n';
    try {
        shh::Epoll epoll(10);

        auto start = std::chrono::steady_clock::now();

        // Should timeout after 100ms since no events
        int result = epoll.wait(-1, 10, 100);  // -1 as dummy listen_fd

        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        std::cout << "Wait result: " << result << " (should be 0 for timeout)" << '\n';
        std::cout << "Wait duration: " << duration.count() << "ms (should be ~100ms)" << '\n';

        if (result == 0 && duration.count() >= 90 && duration.count() <= 150) {
            std::cout << "✓ Epoll wait timeout works correctly" << '\n';
        } else {
            std::cout << "✗ Epoll wait timeout behavior unexpected" << '\n';
        }

    } catch (const std::exception& e) {
        std::cerr << "✗ Epoll wait timeout test failed: " << e.what() << '\n';
    }
}

void test_epoll_with_socket() {
    std::cout << "\n=== Testing Epoll with Socket Integration ===" << '\n';
    try {
        // Create server socket
        shh::Socket sock(AF_INET, SOCK_STREAM, 0, 8081, INADDR_ANY, 10);
        sock.bind_listen();
        int listen_fd = sock.get_sock();

        std::cout << "Server socket created on port 8081, FD: " << listen_fd << '\n';

        // Create epoll
        shh::Epoll epoll(10);

        // Add listen socket to epoll
        epoll.add(listen_fd, EPOLLIN | EPOLLET);
        std::cout << "✓ Added listen socket to epoll" << '\n';

        // Test wait with no connections (should timeout)
        std::cout << "Testing wait with 500ms timeout..." << '\n';
        int events_num = epoll.wait(listen_fd, 10, 500);
        std::cout << "Events returned: " << events_num << " (should be 0)" << '\n';

        if (events_num == 0) {
            std::cout << "✓ Epoll wait correctly timed out with no connections" << '\n';
        } else {
            std::cout << "✗ Unexpected events or behavior" << '\n';
        }

    } catch (const std::exception& e) {
        std::cerr << "✗ Epoll with socket test failed: " << e.what() << '\n';
    }
}

void test_epoll_error_handling() {
    std::cout << "\n=== Testing Epoll Error Handling ===" << '\n';
    try {
        shh::Epoll epoll(10);

        // Test with invalid fd
        std::cout << "Testing add with invalid fd..." << '\n';
        try {
            epoll.add(-1, EPOLLIN);
            std::cout << "✗ Should have failed with invalid fd" << '\n';
        } catch (const std::exception& e) {
            std::cout << "✓ Correctly caught error for invalid fd: " << e.what() << '\n';
        }

        // Test modifying non-existent fd
        std::cout << "Testing mod with non-existent fd..." << '\n';
        int dummy_fd = 999;  // Likely doesn't exist
        epoll.mod(dummy_fd, EPOLLIN);  // Should print warning, not throw
        std::cout << "✓ Handled non-existent fd modification gracefully" << '\n';

        // Test deleting non-existent fd
        std::cout << "Testing del with non-existent fd..." << '\n';
        epoll.del(dummy_fd, EPOLLIN);  // Should print warning, not throw
        std::cout << "✓ Handled non-existent fd deletion gracefully" << '\n';

    } catch (const std::exception& e) {
        std::cerr << "✗ Epoll error handling test failed: " << e.what() << '\n';
    }
}

void test_epoll_handler_logic() {
    std::cout << "\n=== Testing Epoll Handler Logic ===" << '\n';
    try {
        shh::Epoll epoll(10);

        // Create server socket
        shh::Socket sock(AF_INET, SOCK_STREAM, 0, 8082, INADDR_ANY, 10);
        sock.bind_listen();
        int listen_fd = sock.get_sock();

        // Add to epoll
        epoll.add(listen_fd, EPOLLIN | EPOLLET);

        std::cout << "Testing handler with no events..." << '\n';
        epoll.e_handler(listen_fd, 0);  // Should handle gracefully
        std::cout << "✓ Handler processed 0 events without crashing" << '\n';

    } catch (const std::exception& e) {
        std::cerr << "✗ Epoll handler test failed: " << e.what() << '\n';
    }
}

void test_epoll_edge_cases() {
    std::cout << "\n=== Testing Epoll Edge Cases ===" << '\n';
    try {
        // Test with maxEvents = 1
        shh::Epoll epoll_small(1);
        std::cout << "✓ Created epoll with maxEvents=1" << '\n';

        // Test wait with 0 timeout (non-blocking)
        int result = epoll_small.wait(-1, 1, 0);
        std::cout << "Non-blocking wait result: " << result << '\n';

        // Test with large maxEvents
        shh::Epoll epoll_large(1000);
        std::cout << "✓ Created epoll with maxEvents=1000" << '\n';

    } catch (const std::exception& e) {
        std::cerr << "✗ Epoll edge cases test failed: " << e.what() << '\n';
    }
}

void test_request_creation() {
    std::cout << "=== Testing HttpRequest Creation ===" << '\n';
    try {
        shh::HttpRequest request;
        std::cout << "✓ HttpRequest created successfully" << '\n';

        // Test initial state
        if (!request.isComplete()) {
            std::cout << "✓ Initial state is not complete" << '\n';
        } else {
            std::cout << "✗ Initial state should not be complete" << '\n';
        }

        // Test initial values
        if (request.getMethod().empty() && request.getPath().empty() &&
            request.getVersion().empty() && request.getBody().empty()) {
            std::cout << "✓ Initial values are empty" << '\n';
        } else {
            std::cout << "✗ Initial values should be empty" << '\n';
        }

    } catch (const std::exception& e) {
        std::cerr << "✗ HttpRequest creation failed: " << e.what() << '\n';
    }
}

void test_simple_get_request() {
    std::cout << "\n=== Testing Simple GET Request ===" << '\n';
    try {
        shh::HttpRequest request;
        shh::Buffer buffer(1024);

        std::string httpData = "GET /index.html HTTP/1.1\r\nHost: example.com\r\n\r\n";
        buffer.BufferAppend(httpData.c_str(), httpData.length());

        bool result = request.parse(buffer);

        if (result && request.isComplete()) {
            std::cout << "✓ Simple GET request parsed successfully" << '\n';
            std::cout << "Method: " << request.getMethod() << '\n';
            std::cout << "Path: " << request.getPath() << '\n';
            std::cout << "Version: " << request.getVersion() << '\n';
            std::cout << "Host header: " << request.getHeader("host") << '\n';

            if (request.getMethod() == "GET" && request.getPath() == "/index.html" &&
                request.getVersion() == "1.1" && request.getHeader("host") == "example.com") {
                std::cout << "✓ All fields parsed correctly" << '\n';
            } else {
                std::cout << "✗ Some fields parsed incorrectly" << '\n';
            }
        } else {
            std::cout << "✗ Failed to parse simple GET request" << '\n';
        }

    } catch (const std::exception& e) {
        std::cerr << "✗ Simple GET request test failed: " << e.what() << '\n';
    }
}

void test_post_request_with_body() {
    std::cout << "\n=== Testing POST Request with Body ===" << '\n';
    try {
        shh::HttpRequest request;
        shh::Buffer buffer(1024);

        std::string httpData = "POST /api/data HTTP/1.1\r\n"
                              "Host: api.example.com\r\n"
                              "Content-Type: application/json\r\n"
                              "Content-Length: 13\r\n"
                              "\r\n"
                              "{\"key\":\"value\"}";
        buffer.BufferAppend(httpData.c_str(), httpData.length());

        bool result = request.parse(buffer);

        if (result && request.isComplete()) {
            std::cout << "✓ POST request with body parsed successfully" << '\n';
            std::cout << "Method: " << request.getMethod() << '\n';
            std::cout << "Path: " << request.getPath() << '\n';
            std::cout << "Content-Type: " << request.getHeader("content-type") << '\n';
            std::cout << "Content-Length: " << request.getHeader("content-length") << '\n';
            std::cout << "Body: " << request.getBody() << '\n';

            if (request.getMethod() == "POST" && request.getPath() == "/api/data" &&
                request.getHeader("content-type") == "application/json" &&
                request.getHeader("content-length") == "13") {
                std::cout << "✓ POST request fields parsed correctly" << '\n';
            } else {
                std::cout << "✗ POST request fields parsed incorrectly" << '\n';
            }
        } else {
            std::cout << "✗ Failed to parse POST request with body" << '\n';
        }

    } catch (const std::exception& e) {
        std::cerr << "✗ POST request test failed: " << e.what() << '\n';
    }
}

void test_multiple_headers() {
    std::cout << "\n=== Testing Multiple Headers ===" << '\n';
    try {
        shh::HttpRequest request;
        shh::Buffer buffer(1024);

        std::string httpData = "GET /test HTTP/1.1\r\n"
                              "Host: example.com\r\n"
                              "User-Agent: TestClient/1.0\r\n"
                              "Accept: text/html\r\n"
                              "Connection: keep-alive\r\n"
                              "Custom-Header: custom-value\r\n"
                              "\r\n";
        buffer.BufferAppend(httpData.c_str(), httpData.length());

        bool result = request.parse(buffer);

        if (result && request.isComplete()) {
            std::cout << "✓ Multiple headers parsed successfully" << '\n';

            // Test header retrieval
            std::cout << "Host: " << request.getHeader("host") << '\n';
            std::cout << "User-Agent: " << request.getHeader("user-agent") << '\n';
            std::cout << "Accept: " << request.getHeader("accept") << '\n';
            std::cout << "Connection: " << request.getHeader("connection") << '\n';
            std::cout << "Custom-Header: " << request.getHeader("custom-header") << '\n';

            // Test case insensitive header lookup
            if (request.getHeader("HOST") == "example.com" &&
                request.getHeader("User-Agent") == "TestClient/1.0" &&
                request.getHeader("ACCEPT") == "text/html") {
                std::cout << "✓ Case-insensitive header lookup works" << '\n';
            } else {
                std::cout << "✗ Case-insensitive header lookup failed" << '\n';
            }

        } else {
            std::cout << "✗ Failed to parse multiple headers" << '\n';
        }

    } catch (const std::exception& e) {
        std::cerr << "✗ Multiple headers test failed: " << e.what() << '\n';
    }
}

void test_incomplete_request() {
    std::cout << "\n=== Testing Incomplete Request ===" << '\n';
    try {
        shh::HttpRequest request;
        shh::Buffer buffer(1024);

        // Only request line, no headers/body
        std::string httpData = "GET /test HTTP/1.1\r\n";
        buffer.BufferAppend(httpData.c_str(), httpData.length());

        bool result = request.parse(buffer);

        if (!result && !request.isComplete()) {
            std::cout << "✓ Incomplete request correctly identified" << '\n';
        } else {
            std::cout << "✗ Incomplete request should not be marked as complete" << '\n';
        }

        // Add more data
        std::string moreData = "Host: example.com\r\n\r\n";
        buffer.BufferAppend(moreData.c_str(), moreData.length());

        result = request.parse(buffer);

        if (result && request.isComplete()) {
            std::cout << "✓ Request completed after adding more data" << '\n';
        } else {
            std::cout << "✗ Request should be complete after adding headers" << '\n';
        }

    } catch (const std::exception& e) {
        std::cerr << "✗ Incomplete request test failed: " << e.what() << '\n';
    }
}

void test_invalid_requests() {
    std::cout << "\n=== Testing Invalid Requests ===" << '\n';
    try {
        // Test invalid method
        {
            shh::HttpRequest request;
            shh::Buffer buffer(1024);

            std::string httpData = "INVALID /test HTTP/1.1\r\n\r\n";
            buffer.BufferAppend(httpData.c_str(), httpData.length());

            bool result = request.parse(buffer);

            if (!result) {
                std::cout << "✓ Invalid method correctly rejected" << '\n';
            } else {
                std::cout << "✗ Invalid method should be rejected" << '\n';
            }
        }

        // Test invalid path
        {
            shh::HttpRequest request;
            shh::Buffer buffer(1024);

            std::string httpData = "GET invalid-path HTTP/1.1\r\n\r\n";
            buffer.BufferAppend(httpData.c_str(), httpData.length());

            bool result = request.parse(buffer);

            if (!result) {
                std::cout << "✓ Invalid path correctly rejected" << '\n';
            } else {
                std::cout << "✗ Invalid path should be rejected" << '\n';
            }
        }

        // Test malformed request line
        {
            shh::HttpRequest request;
            shh::Buffer buffer(1024);

            std::string httpData = "GET\r\n\r\n";
            buffer.BufferAppend(httpData.c_str(), httpData.length());

            bool result = request.parse(buffer);

            if (!result) {
                std::cout << "✓ Malformed request line correctly rejected" << '\n';
            } else {
                std::cout << "✗ Malformed request line should be rejected" << '\n';
            }
        }

        // Test invalid header format
        {
            shh::HttpRequest request;
            shh::Buffer buffer(1024);

            std::string httpData = "GET /test HTTP/1.1\r\n"
                                  "InvalidHeader\r\n"
                                  "\r\n";
            buffer.BufferAppend(httpData.c_str(), httpData.length());

            bool result = request.parse(buffer);

            if (!result) {
                std::cout << "✓ Invalid header format correctly rejected" << '\n';
            } else {
                std::cout << "✗ Invalid header format should be rejected" << '\n';
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "✗ Invalid requests test failed: " << e.what() << '\n';
    }
}

void test_empty_buffer() {
    std::cout << "\n=== Testing Empty Buffer ===" << '\n';
    try {
        shh::HttpRequest request;
        shh::Buffer buffer(1024);

        bool result = request.parse(buffer);

        if (!result && !request.isComplete()) {
            std::cout << "✓ Empty buffer correctly handled" << '\n';
        } else {
            std::cout << "✗ Empty buffer should return false" << '\n';
        }

    } catch (const std::exception& e) {
        std::cerr << "✗ Empty buffer test failed: " << e.what() << '\n';
    }
}

void test_reset_functionality() {
    std::cout << "\n=== Testing Reset Functionality ===" << '\n';
    try {
        shh::HttpRequest request;
        shh::Buffer buffer(1024);

        // Parse a complete request
        std::string httpData = "GET /test HTTP/1.1\r\nHost: example.com\r\n\r\n";
        buffer.BufferAppend(httpData.c_str(), httpData.length());

        bool result = request.parse(buffer);

        if (result && request.isComplete()) {
            std::cout << "✓ Initial request parsed successfully" << '\n';

            // Reset the request
            request.reset();

            if (!request.isComplete() && request.getMethod().empty() &&
                request.getPath().empty() && request.getVersion().empty() &&
                request.getBody().empty() && request.getHeader("host").empty()) {
                std::cout << "✓ Reset cleared all fields correctly" << '\n';
            } else {
                std::cout << "✗ Reset did not clear all fields" << '\n';
            }

            // Parse a new request
            shh::Buffer newBuffer(1024);
            std::string newHttpData = "POST /api HTTP/1.1\r\nContent-Type: json\r\n\r\n";
            newBuffer.BufferAppend(newHttpData.c_str(), newHttpData.length());

            result = request.parse(newBuffer);

            if (result && request.isComplete() && request.getMethod() == "POST") {
                std::cout << "✓ New request parsed successfully after reset" << '\n';
            } else {
                std::cout << "✗ Failed to parse new request after reset" << '\n';
            }

        } else {
            std::cout << "✗ Failed to parse initial request" << '\n';
        }

    } catch (const std::exception& e) {
        std::cerr << "✗ Reset functionality test failed: " << e.what() << '\n';
    }
}

void test_path_resolution() {
    std::cout << "\n=== Testing Path Resolution ===" << '\n';
    try {
        shh::HttpRequest request;
        shh::Buffer buffer(1024);

        std::string httpData = "GET /index HTTP/1.1\r\nHost: example.com\r\n\r\n";
        buffer.BufferAppend(httpData.c_str(), httpData.length());

        bool result = request.parse(buffer);

        if (result && request.isComplete()) {
            std::cout << "✓ Request parsed successfully" << '\n';
            std::cout << "Original path: " << request.getPath() << '\n';

            // Test path resolution
            std::vector<std::string> files = {"index", "home", "about"};
            request.parse_path(files);

            std::cout << "Resolved path: " << request.getPath() << '\n';

            if (request.getPath() == "/index.html") {
                std::cout << "✓ Path resolution worked correctly" << '\n';
            } else {
                std::cout << "✗ Path resolution failed" << '\n';
            }
        } else {
            std::cout << "✗ Failed to parse request for path resolution test" << '\n';
        }

    } catch (const std::exception& e) {
        std::cerr << "✗ Path resolution test failed: " << e.what() << '\n';
    }
}

void test_header_case_insensitivity() {
    std::cout << "\n=== Testing Header Case Insensitivity ===" << '\n';
    try {
        shh::HttpRequest request;
        shh::Buffer buffer(1024);

        std::string httpData = "GET /test HTTP/1.1\r\n"
                              "Host: example.com\r\n"
                              "Content-Type: application/json\r\n"
                              "USER-AGENT: TestClient/1.0\r\n"
                              "accept: text/html\r\n"
                              "\r\n";
        buffer.BufferAppend(httpData.c_str(), httpData.length());

        bool result = request.parse(buffer);

        if (result && request.isComplete()) {
            std::cout << "✓ Request with mixed case headers parsed successfully" << '\n';

            // Test different case variations
            if (request.getHeader("host") == "example.com" &&
                request.getHeader("HOST") == "example.com" &&
                request.getHeader("Host") == "example.com" &&
                request.getHeader("content-type") == "application/json" &&
                request.getHeader("CONTENT-TYPE") == "application/json" &&
                request.getHeader("user-agent") == "TestClient/1.0" &&
                request.getHeader("USER-AGENT") == "TestClient/1.0" &&
                request.getHeader("accept") == "text/html" &&
                request.getHeader("ACCEPT") == "text/html") {
                std::cout << "✓ Case-insensitive header lookup works for all variations" << '\n';
            } else {
                std::cout << "✗ Case-insensitive header lookup failed for some variations" << '\n';
            }
        } else {
            std::cout << "✗ Failed to parse request with mixed case headers" << '\n';
        }

    } catch (const std::exception& e) {
        std::cerr << "✗ Header case insensitivity test failed: " << e.what() << '\n';
    }
}

void test_edge_cases() {
    std::cout << "\n=== Testing Edge Cases ===" << '\n';
    try {
        // Test request with no headers (just empty line)
        {
            shh::HttpRequest request;
            shh::Buffer buffer(1024);

            std::string httpData = "GET / HTTP/1.1\r\n\r\n";
            buffer.BufferAppend(httpData.c_str(), httpData.length());

            bool result = request.parse(buffer);

            if (result && request.isComplete()) {
                std::cout << "✓ Request with no headers parsed successfully" << '\n';
            } else {
                std::cout << "✗ Request with no headers should be valid" << '\n';
            }
        }

        // Test request with spaces in header values
        {
            shh::HttpRequest request;
            shh::Buffer buffer(1024);

            std::string httpData = "GET /test HTTP/1.1\r\n"
                                  "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64)\r\n"
                                  "\r\n";
            buffer.BufferAppend(httpData.c_str(), httpData.length());

            bool result = request.parse(buffer);

            if (result && request.isComplete()) {
                std::cout << "✓ Request with spaces in header values parsed successfully" << '\n';
                std::cout << "User-Agent: " << request.getHeader("user-agent") << '\n';
            } else {
                std::cout << "✗ Request with spaces in header values should be valid" << '\n';
            }
        }

        // Test non-existent header
        {
            shh::HttpRequest request;
            shh::Buffer buffer(1024);

            std::string httpData = "GET /test HTTP/1.1\r\nHost: example.com\r\n\r\n";
            buffer.BufferAppend(httpData.c_str(), httpData.length());

            request.parse(buffer);

            std::string nonExistentHeader = request.getHeader("non-existent");
            if (nonExistentHeader.empty()) {
                std::cout << "✓ Non-existent header returns empty string" << '\n';
            } else {
                std::cout << "✗ Non-existent header should return empty string" << '\n';
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "✗ Edge cases test failed: " << e.what() << '\n';
    }
}

int main() {
    std::cout << "Starting Tests..." << '\n';

    test_epoll_creation();
    test_epoll_add_remove();
    test_epoll_wait_timeout();
    test_epoll_with_socket();
    test_epoll_error_handling();
    test_epoll_handler_logic();
    test_epoll_edge_cases();
    test_request_creation();
    test_simple_get_request();
    test_post_request_with_body();
    test_multiple_headers();
    test_incomplete_request();
    test_invalid_requests();
    test_empty_buffer();
    test_reset_functionality();
    test_path_resolution();
    test_header_case_insensitivity();
    test_edge_cases();
    std::cout << "\n=== All Tests Completed ===" << '\n';
    return 0;
}
