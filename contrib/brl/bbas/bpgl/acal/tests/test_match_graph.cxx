#include "testlib/testlib_test.h"

#include <acal/acal_match_graph.h>

static void test_match_graph()
{
  // eqaulity tests
  match_params mp1(10, 10, 0.5f, 0.5f), mp2(10, 10, 0.5f, 0.5f), mp3;
  TEST("match_params equality", mp1, mp2);
  TEST("match_params inequality", mp1 != mp3, true);

}

TESTMAIN(test_match_graph);
