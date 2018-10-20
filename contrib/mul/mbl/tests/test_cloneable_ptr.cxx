#include <iostream>
#include <testlib/testlib_test.h>
// not used? #include <vcl_compiler.h>
#include <mbl/mbl_cloneable_ptr.h>

struct some_class
{
  static int count;
  some_class() { ++count; }
  ~some_class() { --count; }
  some_class* clone() const { return new some_class; }
};

int some_class::count = 0;


static void test_cloneable_ptr()
{
  mbl_cloneable_ptr<some_class> ptr1;
  mbl_cloneable_ptr<some_class> ptr2;
  TEST( "Initial count = 0", some_class::count, 0 );

  ptr1 = new some_class();
  ptr2 = new some_class() ;
  TEST( "New count = 2", some_class::count, 2 );

  { // Test construction and destruction of cloneable_ptr
    mbl_cloneable_ptr<some_class> ptr3a;
    ptr3a = new some_class();
    TEST( "Nested count = 3", some_class::count, 3 );
  }
  TEST( "Back again, count = 2", some_class::count, 2 );

  { // Test release and empty destruction of cloneable_ptr
    some_class* true_ptr_3b=nullptr;
    {
      mbl_cloneable_ptr<some_class> ptr3b = true_ptr_3b = new some_class();
      TEST( "Nested count = 3", some_class::count, 3 );
      some_class* released_ptr_3b = ptr3b.release();
      TEST( "Released == original pointer 3b", true_ptr_3b, released_ptr_3b);
      TEST( "Released count = 3", some_class::count, 3 );
    }
    TEST( "No deletion of released ptr, count = 3)", some_class::count, 3 );
    delete true_ptr_3b;
    TEST( "Manual deletion of released ptr, count = 2)", some_class::count, 2 );
    // Check assumption that destructor isn't called on null pointer.
    true_ptr_3b=nullptr;
    delete true_ptr_3b;
    TEST( "Manual deletion of null ptr, count = 2)", some_class::count, 2 );
  }

  { // Test construction and destruction of cloneable_nzptr
    mbl_cloneable_nzptr<some_class> ptr3c(new some_class());
    TEST( "Nested count = 3", some_class::count, 3 );
  }
  TEST( "Back again, count = 2", some_class::count, 2 );

  { // Test release and empty destruction of cloneable_nzptr
    some_class* true_ptr_3d=nullptr;
    {
      mbl_cloneable_nzptr<some_class> ptr3d(true_ptr_3d=new some_class());
      TEST( "Nested count = 3", some_class::count, 3 );
      some_class* released_ptr_3d = ptr3d.replace(new some_class());
      TEST( "Released == original pointer 3", true_ptr_3d, released_ptr_3d);
      TEST( "Replaced count = 4", some_class::count, 4 );
    }
    TEST( "Replaced ptr deleted, count = 3", some_class::count, 3 );
    delete true_ptr_3d;
    TEST( "Manual deletion of released ptr, count = 2)", some_class::count, 2 );
  }

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

  ptr1 = nullptr;
  TEST( "Down to count = 1", some_class::count, 1 );
}


TESTMAIN(test_cloneable_ptr);
