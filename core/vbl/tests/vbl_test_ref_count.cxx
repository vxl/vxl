#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vbl/vbl_ref_count.h>

class myclass_test1 : public vbl_ref_count
{
};

class myclass_test2 : public vbl_ref_count
{
  int value;
public:
  myclass_test2() : value(4) { }
  myclass_test2( myclass_test2 const& o ) : value(o.value)  {
  }
};

static void vbl_test_ref_count()
{
  // test that ref counts are handled correctly when copying objects.

  // Class with compiler generated copy constructor
  {
    myclass_test1* obj1 = new myclass_test1();
    // Get three references to the object, like creating three smart pointers would do.
    obj1->ref();
    obj1->ref();
    obj1->ref();

    TEST( "Original has 3 references", obj1->get_references(), 3 );

    // obj2 is a new object. Nothing holds references to it yet.
    myclass_test1* obj2 = new myclass_test1( *obj1 );

    TEST( "Copy has 0 references", obj2->get_references(), 0 );

    delete obj1;
    delete obj2;
  }

  // Class with user-defined copy constructor
  {
    myclass_test2* obj1 = new myclass_test2();
    // Get three references to the object, like creating three smart pointers would do.
    obj1->ref();
    obj1->ref();
    obj1->ref();

    TEST( "Original has 3 references", obj1->get_references(), 3 );

    // obj2 is a new object. Nothing holds references to it yet.
    myclass_test2* obj2 = new myclass_test2( *obj1 );

    TEST( "Copy has 0 references", obj2->get_references(), 0 );

    delete obj1;
    delete obj2;
  }
}

TESTMAIN(vbl_test_ref_count);
