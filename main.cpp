#include "matching_engine.h"
#include "network.h"
#include "spsc_queue.h"
#include "order.h"
#include <thread>
#include <iostream>

int main() {
    SPSCQueue<Order, 1024> queue;
    MatchingEngine engine(queue);
    NetworkServer server(9000, queue);
    std::thread matching_thread(&MatchingEngine::run, &engine);
    std::thread network_thread(&NetworkServer::run, &server);
    std::cout << "Matching Engine + Network Server started.\n";
    std::cout << "Connect using: nc localhost 9000\n";
    std::cout << "Format: B 100 10   or   S 100 10\n\n";
    network_thread.join();
    engine.stop();
    matching_thread.join();
    return 0;
}
 
