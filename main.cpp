#include <thread>
#include "order.h"
#include "spsc_queue.h"
#include "matching_engine.h"
#include "network.h"

int main() {
    SPSCQueue<Order, 1024> queue;
    MatchingEngine engine(queue);

    std::thread matching_thread(&MatchingEngine::run, &engine);

    NetworkServer server(9000, queue);
    server.run();

    matching_thread.join();
    return 0;
}