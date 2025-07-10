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

int main() {
    std::cout << "Starting Tests..." << '\n';

    test_epoll_creation();
    test_epoll_add_remove();
    test_epoll_wait_timeout();
    test_epoll_with_socket();
    test_epoll_error_handling();
    test_epoll_handler_logic();
    test_epoll_edge_cases();

    std::cout << "\n=== All Tests Completed ===" << '\n';
    return 0;
}
