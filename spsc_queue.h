#pragma once
#include <atomic>
#include <array>
#include <utility>
#include <cstddef>
template<typename T,std::size_t Size> 
class SPSCQueue{
private:
  std::array<T,Size> buffer;
  std::atomic<std::size_t> head{0};
  std::atomic<std::size_t> tail{0};
public:
 bool push(T&& item){
  std::size_t current_head=head.load(std::memory_order_relaxed);
    std::size_t next_head=(current_head+1)%Size;
      if(next_head==tail.load(std::memory_order_acquire))return false;
     buffer[current_head]=std::move(item);
      head.store(next_head,std::memory_order_release);
     return true;
  }

bool pop(T& item){
   std::size_t current_tail=tail.load(std::memory_order_relaxed);
   if(current_tail==head.load(std::memory_order_acquire))return false;
   std::size_t next_tail=(current_tail+1)%Size;
   item=std::move(buffer[current_tail]);
    tail.store(next_tail,std::memory_order_release);
     return true;
  }

//so this is very very important to jsut understand this idea right here

};