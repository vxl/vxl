/*
  fsm
*/
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vbl/vbl_shared_pointer.h>

#define print vcl_cout

struct some_class
{
  some_class() { print << "ctor" << vcl_endl; }
  ~some_class() { print << "dtor" << vcl_endl; }
};

static void test_int()
{
  typedef vbl_shared_pointer<int> pi;

#if VCL_HAS_MEMBER_TEMPLATES
  pi a(13);
  pi b(24);
#else
  pi a(new int(13));
  pi b(new int(24));
#endif

  print << "*a = " << *a << vcl_endl;
  print << "*b = " << *b << vcl_endl;

  for (int i=0; i<10003; ++i) {
    pi tmp = a;
    a = b;
    b = tmp;
  }

  print << "*a = " << *a << vcl_endl;
  print << "*b = " << *b << vcl_endl;
}

static void test_class()
{
  typedef vbl_shared_pointer<some_class> sp;

  sp a(new some_class);
  sp b((some_class*)0);

  print << "a = " << (void*)a.as_pointer() << vcl_endl;
  print << "b = " << (void*)b.as_pointer() << vcl_endl;

  for (int i=0; i<10003; ++i) {
    sp tmp = a;
    a = b;
    b = tmp;
  }

  print << "a = " << (void*)a.as_pointer() << vcl_endl;
  print << "b = " << (void*)b.as_pointer() << vcl_endl;
}

static void vbl_test_shared_pointer()
{
  test_int();
  test_class();
}

TESTMAIN(vbl_test_shared_pointer);
