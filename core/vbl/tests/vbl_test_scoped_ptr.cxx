/*
  fsm
*/
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vbl/vbl_scoped_ptr.h>

struct some_class
{
  static int count;
  some_class() { ++count; }
  ~some_class() { --count; }
};
int some_class::count = 0;

// This should fail to compile.
// struct incomplete;
// vbl_scoped_ptr<incomplete> y;

static void vbl_test_scoped_ptr()
{
  vbl_scoped_ptr<some_class> ptr1;
  vbl_scoped_ptr<some_class> ptr2;

  TEST( "Initial count = 0", some_class::count, 0 );

  ptr1.reset( new some_class() );
  ptr2.reset( new some_class() );

  TEST( "New count = 2", some_class::count, 2 );

  {
    vbl_scoped_ptr<some_class> ptr3( new some_class() );
    TEST( "Nested count = 3", some_class::count, 3 );
  }

  TEST( "Back again, count = 2", some_class::count, 2 );

  ptr2.reset();

  TEST( "Down to count = 1", some_class::count, 1 );
}

TESTMAIN(vbl_test_scoped_ptr);
