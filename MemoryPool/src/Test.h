#pragma once
#include "Allocator.h"

namespace Test 
  {

  void simple_test()
    {
    std::cout << "\n Starting simple test... \n ";
    Allocator a(4096);

    int* a_1int = (int*)a.allocate(sizeof(int));
    double* a_2double = (double*)a.allocate(sizeof(double));
    char* a_3char = (char*)a.allocate(sizeof(char));

    *a_1int = 100;
    *a_2double = 52.0123456789;
    *a_3char = 'T';

    std::cout << (*a_1int == 100 ? "passed " : "failed") << std::endl;
    std::cout << (*a_2double == 52.0123456789 ? "passed " : "failed") << std::endl;
    std::cout << (*a_3char == 'T' ? "passed " : "failed") << std::endl;

    a.deallocate(a_1int);
    a.deallocate(a_3char);
    std::cout << (*a_2double == 52.0123456789 ? "passed " : "failed") << std::endl;
    a.deallocate(a_2double);

    double* b_1double = (double*)a.allocate(sizeof(double));
    *b_1double = 1.111111111;

    std::cout << (*b_1double == 1.111111111 ? "passed " : "failed") << std::endl;
    std::cout << (a_1int == nullptr ? "passed " : "failed") << std::endl;
    std::cout << (a_2double == nullptr ? "passed " : "failed") << std::endl;
    std::cout << (a_3char == nullptr ? "passed " : "failed") << std::endl;

    }
  }
