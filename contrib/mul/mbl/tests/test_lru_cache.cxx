#include <vcl_iostream.h>
#include <mbl/mbl_lru_cache.h>
#include <testlib/testlib_test.h>

void test_lru_cache()
{
  vcl_cout << "\n\n***********************\n"
           <<     " Testing mbl_lru_cache\n"
           <<     "***********************\n";

  mbl_lru_cache<unsigned int, int> cache(3);

  cache.insert(1, -1);
  cache.insert(2, -2);
  cache.insert(3, -3);

  TEST("Lookup 2 hits -1",*cache.lookup(2), -2);
  TEST("Lookup 0 miss",cache.lookup(0), 0);
  cache.insert(4, -4);
  TEST("Lookup 1 miss",cache.lookup(1), 0);
  cache.insert(5, -5);
  TEST("Lookup 3 miss",cache.lookup(3), 0);
  TEST("Lookup 2 hits -2",*cache.lookup(2), -2);
}

TESTMAIN(test_lru_cache);
