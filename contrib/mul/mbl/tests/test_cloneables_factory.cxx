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

  mbl_cloneables_factory<mbl_test_cf_base>::add(mbl_test_cf_A());
  mbl_cloneables_factory<mbl_test_cf_base>::add(mbl_test_cf_B());
  mbl_cloneables_factory<mbl_test_cf_base>::add(mbl_test_cf_A(), "wibble");
  
  mbl_test_cf_base * p1 = mbl_cloneables_factory<mbl_test_cf_base>::get("mbl_test_cf_A");
  mbl_test_cf_base * p2 = mbl_cloneables_factory<mbl_test_cf_base>::get("mbl_test_cf_B");
  mbl_test_cf_base * p3 = mbl_cloneables_factory<mbl_test_cf_base>::get("wibble");

  TEST("get A",dynamic_cast<mbl_test_cf_A*>(p1)!=0,true);
  TEST("get B",dynamic_cast<mbl_test_cf_B*>(p2)!=0,true);
  TEST("!get B",dynamic_cast<mbl_test_cf_A*>(p2)==0,true);
  TEST("get wibble",dynamic_cast<mbl_test_cf_A*>(p3)!=0,true);
  
  delete p1;
  delete p2;
  delete p3;
}

TESTMAIN(test_cloneables_factory);
