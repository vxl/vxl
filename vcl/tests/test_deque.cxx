#include <iostream>
#include <deque>
#include <vcl_compiler.h>

int test_deque_main(int /*argc*/,char* /*argv*/[])
{
  typedef std::deque<int> mydeque;
  mydeque dq;

  dq.push_front(2);
  dq.push_back(3);
  dq.push_front(1);

  for (int & p : dq)
    std::cout << p << std::endl;

  return 0;
}
