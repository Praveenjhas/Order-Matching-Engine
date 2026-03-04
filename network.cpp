#include "network.h"

#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <chrono>

static void set_non_blocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

NetworkServer::NetworkServer(int p, SPSCQueue<Order, 1024>& q)
    : port(p), queue(q) {
    setup_server();
}

void NetworkServer::setup_server() {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    bind(server_fd, (sockaddr*)&addr, sizeof(addr));
    listen(server_fd, SOMAXCONN);
    set_non_blocking(server_fd);

    epoll_fd = epoll_create1(0);

    epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev);

    std::cout << "Server listening on port " << port << "\n";
}

void NetworkServer::run() {
    epoll_event events[10];
    uint64_t order_id = 1;

    while (true) {
        int n = epoll_wait(epoll_fd, events, 10, -1);

        for (int i = 0; i < n; ++i) {
            int fd = events[i].data.fd;

            if (fd == server_fd) {
                int client_fd = accept(server_fd, nullptr, nullptr);
                set_non_blocking(client_fd);

                epoll_event ev{};
                ev.events = EPOLLIN;
                ev.data.fd = client_fd;

                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);

                std::cout << "Client connected\n";
            }
            else {
                char buffer[128];
                int bytes = read(fd, buffer, sizeof(buffer) - 1);

                if (bytes <= 0) {
                    close(fd);
                    continue;
                }

                buffer[bytes] = '\0';

                char side_char;
                int price, qty;

                if (sscanf(buffer, "%c %d %d", &side_char, &price, &qty) == 3) {

                    Order o;
                    o.id = order_id++;
                    o.side = (side_char == 'B') ? Side::Buy : Side::Sell;
                    o.price = price;
                    o.quantity = qty;

                    auto now = std::chrono::steady_clock::now();
                    o.timestamp =
                        std::chrono::duration_cast<std::chrono::nanoseconds>(
                            now.time_since_epoch()).count();

                    while (!queue.push(std::move(o))) {}
                }
            }
        }
    }
}