#pragma once
#include "spsc_queue.h"
#include "order.h"
class NetworkServer {
public:
    NetworkServer(int port, SPSCQueue<Order, 1024>& queue);
    void run();

private:
    int port;
    int server_fd;
    int epoll_fd;
    SPSCQueue<Order, 1024>& queue;

    void setup_server();
};