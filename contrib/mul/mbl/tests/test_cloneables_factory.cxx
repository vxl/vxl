// This is mul/mbl/tests/test_cloneables_factory.cxx
#include <mbl/mbl_cloneables_factory.h>
#include <testlib/testlib_test.h>


class mbl_test_cf_base
{
public:
  virtual ~mbl_test_cf_base() {};
  virtual vcl_string is_a() const=0;
  virtual mbl_test_cf_base* clone() const=0;
};

class mbl_test_cf_A : public mbl_test_cf_base
{
public:
  virtual ~mbl_test_cf_A() {};
  virtual vcl_string is_a() const { return vcl_string("mbl_test_cf_A"); }
  virtual mbl_test_cf_base* clone() const { return new mbl_test_cf_A(*this); }
};

class mbl_test_cf_B : public mbl_test_cf_base
{
public:
  virtual ~mbl_test_cf_B() {};
  virtual vcl_string is_a() const { return vcl_string("mbl_test_cf_B"); }
  virtual mbl_test_cf_base* clone() const { return new mbl_test_cf_B(*this); }
};

MBL_CLONEABLES_FACTORY_INSTANTIATE(mbl_test_cf_base);




void test_cloneables_factory()
{
  vcl_cout << "**********************************\n"
           << "  Testing mbl_cloneables_factory\n"
           << "**********************************\n";

// Add objects using their default is_a() names.
  mbl_cloneables_factory<mbl_test_cf_base>::add(mbl_test_cf_A());
  mbl_cloneables_factory<mbl_test_cf_base>::add(mbl_test_cf_B());
// Add object using arbitrary name.
  mbl_cloneables_factory<mbl_test_cf_base>::add(mbl_test_cf_A(), "wibble");
  
// Check we can get all objects.
  {
    vcl_auto_ptr<mbl_test_cf_base> p = mbl_cloneables_factory<mbl_test_cf_base>::get_clone("mbl_test_cf_A");
    TEST("get A == A",dynamic_cast<mbl_test_cf_A*>(p.get())!=0,true);
  }
  {
    vcl_auto_ptr<mbl_test_cf_base> p = mbl_cloneables_factory<mbl_test_cf_base>::get_clone("mbl_test_cf_B");
    TEST("get B == B",dynamic_cast<mbl_test_cf_B*>(p.get())!=0,true);
// Check the tests would fail if there was a problem.
    TEST("get B != A",dynamic_cast<mbl_test_cf_A*>(p.get())==0,true);
  }
  {
    vcl_auto_ptr<mbl_test_cf_base> p = mbl_cloneables_factory<mbl_test_cf_base>::get_clone("wibble");
    TEST("get wibble == A",dynamic_cast<mbl_test_cf_A*>(p.get())!=0,true);
  }

// Test the error reporting mechanism  
#if VCL_HAS_EXCEPTIONS
  {
    testlib_test_begin("!get foo");
    vcl_auto_ptr<mbl_test_cf_base> p;
    bool caught_error = false;
    try { p = mbl_cloneables_factory<mbl_test_cf_base>::get_clone("foo");  }
    catch (const mbl_exception_no_name_in_factory &) { caught_error=true; }
    testlib_test_perform(caught_error);
  }
#endif

}

TESTMAIN(test_cloneables_factory);
