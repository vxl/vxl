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

  pi a(new int(13));
  pi b(new int(24));

  print << "*a = " << *a << vcl_endl;
  print << "*b = " << *b << vcl_endl;

  for (int i=0; i<10003; ++i) {
    pi tmp = a;
    a = b;
    b = tmp;
  }

  TEST( "Swap (a,b)  [1]", *a, 24 );
  TEST( "Swap (a,b)  [1]", *b, 13 );
}

static void test_class()
{
  typedef vbl_shared_pointer<some_class> sp;

  sp a(new some_class);
  sp b((some_class*)0);

  void* olda = (void*)a.as_pointer();
  void* oldb = (void*)b.as_pointer();

  for (int i=0; i<10003; ++i) {
    sp tmp = a;
    a = b;
    b = tmp;
  }

  TEST( "Swap (a,b)  [1]", (void*)a.as_pointer(), oldb );
  TEST( "Swap (a,b)  [2]", (void*)b.as_pointer(), olda );
}


struct base_class
{
  virtual ~base_class() { };
  virtual int who() const { return 0; }
};

struct derv_class1
  : public base_class
{
  static int cnt;
  derv_class1() { ++cnt; }
  ~derv_class1() { --cnt; }
  virtual int who() const { return 1; }
};
int derv_class1::cnt = 0;

struct derv_class2
  : public base_class
{
  static int cnt;
  derv_class2() { ++cnt; }
  ~derv_class2() { --cnt; }
  virtual int who() const { return 2; }
};
int derv_class2::cnt = 0;


static void test_derived_class()
{
  typedef vbl_shared_pointer<derv_class1> d1p;
  typedef vbl_shared_pointer<derv_class2> d2p;
  typedef vbl_shared_pointer<base_class> bp;

  {
    vcl_cout << "Construct with raw derived pointer\n";
    {
      bp p( new derv_class1 );
      TEST( "  Points to derived", p->who(), 1 );
      TEST( "  Object exists", derv_class1::cnt, 1 );
    }
    TEST( "  Destruction", derv_class1::cnt, 0 );
  }

  {
    vcl_cout << "Construct with derived smart pointer\n";
    {
      d1p dp( new derv_class1 );
      bp p( dp );
      TEST( "  Points to derived", p->who(), 1 );
      TEST( "  Object exists", derv_class1::cnt, 1 );
    }
    TEST( "  Destruction", derv_class1::cnt, 0 );
  }

  {
    vcl_cout << "Assign with derived smart pointer\n";
    {
      bp p;
      {
        d1p dp( new derv_class1 );
        p = dp;
      }
      TEST( "  Points to derived", p->who(), 1 );
      TEST( "  Object exists", derv_class1::cnt, 1 );
    }
    TEST( "  Destruction", derv_class1::cnt, 0 );
  }

  {
    vcl_cout << "Base sptr can point to any derived\n";
    {
      bp p( new derv_class1 );
      TEST( "  Object exists", derv_class1::cnt, 1 );
      p = d2p( new derv_class2 );
      TEST( "  Object 1 d.n.e.", derv_class1::cnt, 0 );
      TEST( "  Object 2 exists", derv_class2::cnt, 1 );
    }
    TEST( "  Destruction", derv_class2::cnt, 0 );
  }

}

static void vbl_test_shared_pointer()
{
  test_int();
  test_class();
  test_derived_class();
}

TESTMAIN(vbl_test_shared_pointer);
