// This is mul/mbl/tests/test_priority_bounded_queue.cxx
#include <functional>
#include <iostream>
#include <deque>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mbl/mbl_priority_bounded_queue.h>
#include <testlib/testlib_test.h>

void test_priority_bounded_queue()
{
  std::cout << "************************************\n"
           << " Testing mbl_priority_bounded_queue\n"
           << "************************************\n";

  mbl_priority_bounded_queue<int, std::deque<int>, std::greater<int> > pq(3);

  pq.push(6);
  pq.push(4);
  pq.push(2);
  pq.push(0);

  TEST("Priority queue is size 3, after adding 4 elements", pq.size(), 3);
  TEST("Priority queue top() is correct ", pq.top(), 2);
  pq.pop();
  TEST("After 1 pop, Priority queue top() is correct ", pq.top(), 4);
  pq.pop();
  TEST("After 2 pops, Priority queue top() is correct ", pq.top(), 6 );
  pq.pop();
  TEST("After 3 pops, Priority queue now empty ", pq.size(), 0);
}

TESTMAIN(test_priority_bounded_queue);
