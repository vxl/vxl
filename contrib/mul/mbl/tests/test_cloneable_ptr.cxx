#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <mbl/mbl_cloneable_ptr.h>

struct some_class
{
  static int count;
  some_class() { ++count; }
  ~some_class() { --count; }
  some_class* clone() const { return new some_class; } 
};
int some_class::count = 0;

// This should fail to compile.
// struct incomplete;
// vbl_scoped_ptr<incomplete> y;

static void test_cloneable_ptr()
{
  mbl_cloneable_ptr<some_class> ptr1;
  mbl_cloneable_ptr<some_class> ptr2;
  TEST( "Initial count = 0", some_class::count, 0 );

  ptr1 = new some_class();
  ptr2 = new some_class() ;
  TEST( "New count = 2", some_class::count, 2 );

  {
    mbl_cloneable_ptr<some_class> ptr3;
    ptr3 = new some_class();
    TEST( "Nested count = 3", some_class::count, 3 );
  }
  TEST( "Back again, count = 2", some_class::count, 2 );

  {
    mbl_cloneable_ptr<some_class> ptr3(new some_class());
    TEST( "Nested count = 3", some_class::count, 3 );
  }

  {
    mbl_cloneable_nzptr<some_class> ptr3(new some_class());
    TEST( "Nested count = 3", some_class::count, 3 );
  }
  TEST( "Back again, count = 2", some_class::count, 2 );

  {
    some_class sc;
    mbl_cloneable_ptr<some_class> ptr4;
    ptr4 = sc;
    TEST( "New count = 4", some_class::count, 4 );
  }
  TEST( "Back again, count = 2", some_class::count, 2 );

  {
    mbl_cloneable_ptr<some_class> ptr4(*ptr1);
    TEST( "New count = 3", some_class::count, 3 );
  }
  TEST( "Back again, count = 2", some_class::count, 2 );

  {
    mbl_cloneable_nzptr<some_class> ptr4(*ptr1);
    TEST( "New count = 3", some_class::count, 3 );
  }
  TEST( "Back again, count = 2", some_class::count, 2 );

  ptr1 = 0;
  TEST( "Down to count = 1", some_class::count, 1 );

}


TESTMAIN(test_cloneable_ptr);
