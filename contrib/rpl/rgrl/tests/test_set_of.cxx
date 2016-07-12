#include <testlib/testlib_test.h>

#include <rgrl/rgrl_set_of.h>
#include <rgrl/rgrl_set_of.hxx>

static void test_set_of()
{
  rgrl_set_of<double> data;
  rgrl_set_of<double> const& data_const = data;

  data.push_back( 1.0 );
  data.push_back( 2.0 );
  data.push_back( 3.0 );
  TEST("Push back" , data_const.size(), 3);
  TEST("Access", data[0] == 1.0 && data_const[2] == 3.0, true);

  data[1] = 4.0;
  TEST("L-value access", data[1] == 4.0 && data_const[1] == 4.0, true);

  data.resize( 10 );
  TEST("Resize" , data.size() == 10 &&
                  data[0] == 1.0 &&
                  data[1] == 4.0 &&
                  data[2] == 3.0 , true);

  data.clear();
  TEST("Clear", data.size(), 0 );
}

TESTMAIN(test_set_of);
