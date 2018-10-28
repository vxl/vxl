// This is mul/mbl/tests/test_cloneables_factory.cxx
#include <mbl/mbl_cloneables_factory_def.h>
#include <testlib/testlib_test.h>


class mbl_test_cf_base
{
 public:
  virtual ~mbl_test_cf_base() = default;
  virtual std::string is_a() const=0;
  virtual mbl_test_cf_base* clone() const=0;
};

class mbl_test_cf_A : public mbl_test_cf_base
{
 public:
  ~mbl_test_cf_A() override = default;
  std::string is_a() const override { return std::string("mbl_test_cf_A"); }
  mbl_test_cf_base* clone() const override { return new mbl_test_cf_A(*this); }
};

class mbl_test_cf_B : public mbl_test_cf_base
{
 public:
  ~mbl_test_cf_B() override = default;
  std::string is_a() const override { return std::string("mbl_test_cf_B"); }
  mbl_test_cf_base* clone() const override { return new mbl_test_cf_B(*this); }
};

MBL_CLONEABLES_FACTORY_INSTANTIATE(mbl_test_cf_base);

#include <mbl/mbl_cloneables_factory.h>

void test_cloneables_factory()
{
  std::cout << "**********************************\n"
           << "  Testing mbl_cloneables_factory\n"
           << "**********************************\n";

  // Add objects using their default is_a() names.
  mbl_cloneables_factory<mbl_test_cf_base>::add(mbl_test_cf_A());
  mbl_cloneables_factory<mbl_test_cf_base>::add(mbl_test_cf_B());
  // Add object using arbitrary name.
  mbl_cloneables_factory<mbl_test_cf_base>::add(mbl_test_cf_A(), "wibble");

  // Check we can get all objects.
  {
    std::unique_ptr<mbl_test_cf_base> p = mbl_cloneables_factory<mbl_test_cf_base>::get_clone("mbl_test_cf_A");
    TEST("get A == A",dynamic_cast<mbl_test_cf_A*>(p.get())!=nullptr,true);
  }
  {
    std::unique_ptr<mbl_test_cf_base> p = mbl_cloneables_factory<mbl_test_cf_base>::get_clone("mbl_test_cf_B");
    TEST("get B == B",dynamic_cast<mbl_test_cf_B*>(p.get())!=nullptr,true);
    // Check the tests would fail if there was a problem.
    TEST("get B != A",dynamic_cast<mbl_test_cf_A*>(p.get())==nullptr,true);
  }
  {
    std::unique_ptr<mbl_test_cf_base> p = mbl_cloneables_factory<mbl_test_cf_base>::get_clone("wibble");
    TEST("get wibble == A",dynamic_cast<mbl_test_cf_A*>(p.get())!=nullptr,true);
  }

// Test the error reporting mechanism
  {
    testlib_test_begin("!get foo");
    std::unique_ptr<mbl_test_cf_base> p;
    bool caught_error = false;
    try { p = mbl_cloneables_factory<mbl_test_cf_base>::get_clone("foo");  }
    catch (const mbl_exception_no_name_in_factory &) { caught_error=true; }
    testlib_test_perform(caught_error);
  }
}

TESTMAIN(test_cloneables_factory);
