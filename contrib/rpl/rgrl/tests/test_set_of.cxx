#include <testlib/testlib_test.h>

#include <rgrl/rgrl_set_of.h>
#include <rgrl/rgrl_set_of.txx>

MAIN( test_set_of )
{
  START( "set of <T>" );

  rgrl_set_of<double> data;
  rgrl_set_of<double> const& data_const = data;

  testlib_test_begin( "Push back" );
  data.push_back( 1.0 );
  data.push_back( 2.0 );
  data.push_back( 3.0 );
  testlib_test_perform( data_const.size() == 3 );

  testlib_test_begin( "Access" );
  testlib_test_perform( data[0] == 1.0 && data_const[2] == 3.0 );

  testlib_test_begin( "L-value access" );
  data[1] = 4.0;
  testlib_test_perform( data[1] == 4.0 && data_const[1] == 4.0 );

  testlib_test_begin( "Resize" );
  data.resize( 10 );
  testlib_test_perform( data.size() == 10 &&
                        data[0] == 1.0 &&
                        data[1] == 4.0 &&
                        data[2] == 3.0 );

  data.clear();
  TEST( "Clear", data.size(), 0 );

  SUMMARY();
}
