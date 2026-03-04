#include "matching_engine.h"
#include <chrono>
#include <iostream>
#include <algorithm>

MatchingEngine::MatchingEngine(SPSCQueue<Order,1024>& q): queue( q){}

void MatchingEngine::run(){
    Order order;
    while(running.load(std::memory_order_relaxed)){
      if(queue.pop(order)){
          auto now=std::chrono::steady_clock::now();
         uint64_t now_ns=std::chrono::duration_cast<std::chrono::nanoseconds>(
               now.time_since_epoch()).count();
         uint64_t latency_ns=now_ns- order.timestamp;
         min_latency=std::min(min_latency,latency_ns);
         max_latency=std::max(max_latency,latency_ns);
          total_latency+=latency_ns;
          count++;
        std::cout<<"Latency(us): "
                  <<latency_ns/1000.0
                  <<"\n";
            book.add_order(order);
       }
     
    
    }
     if (count > 0) {
        std::cout << "\nLatency Stats:\n";
        std::cout << "Min (us): " << min_latency / 1000.0 << "\n";
        std::cout << "Max (us): " << max_latency / 1000.0 << "\n";
        std::cout << "Avg (us): "
                  << (total_latency / count) / 1000.0
                  << "\n";}

}


void MatchingEngine::stop(){
  running.store(false,std::memory_order_relaxed);
    }

