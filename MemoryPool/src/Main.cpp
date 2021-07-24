#include <iostream>
#include <chrono>
#include "Allocator.h"

int main()
{
    std::cout << "A small benchmark. Run in release mode\n";

    Allocator a(65536);
    size_t alloc_size = 4096;
    size_t repeats = 10000000;

    auto start1 = std::chrono::system_clock::now();
    for (size_t i = 0; i < repeats; i++) // worst case of alloc/dealloc that invokes a lot of calls inside Allocator
      {
      void* a1 = a.allocate(alloc_size);
      void* a2 = a.allocate(alloc_size);
      void* a3 = a.allocate(alloc_size);
      void* a4 = a.allocate(alloc_size);
      void* a5 = a.allocate(alloc_size);

      a.deallocate(a2); 
      a.deallocate(a4);
      a.deallocate(a3);

      void* a6 = a.allocate(alloc_size);
      void* a7 = a.allocate(alloc_size);
      void* a8 = a.allocate(alloc_size);

      a.deallocate(a6);
      a.deallocate(a8);

      a.deallocate(a7);
      a.deallocate(a1);
      a.deallocate(a5);
      }
    auto end1 = std::chrono::system_clock::now();
    std::chrono::duration<int64_t, std::nano> time1 = end1 - start1;
    std::cout << "...working with allocator\n "
      << time1.count() / 1000000 << " ms\n";

    auto start3 = std::chrono::system_clock::now();
    for (size_t i = 0; i < repeats; i++) // just a copypaste of alloc/dealloc logic from previous case
      {
      void* a1 = malloc(alloc_size);
      void* a2 = malloc(alloc_size);
      void* a3 = malloc(alloc_size);
      void* a4 = malloc(alloc_size);
      void* a5 = malloc(alloc_size);
                        
      free(a2);
      free(a4);
      free(a3);

      void* a6 = malloc(alloc_size);
      void* a7 = malloc(alloc_size);
      void* a8 = malloc(alloc_size);

      free(a6);
      free(a8);
              
      free(a7);
      free(a1);
      free(a5);
      }
    auto end3 = std::chrono::system_clock::now();
    std::chrono::duration<int64_t, std::nano> time3 = end3 - start3;
    std::cout << "...working with malloc/free \n "
      << time3.count() / 1000000 << " ms\n";

    system("pause");
}

