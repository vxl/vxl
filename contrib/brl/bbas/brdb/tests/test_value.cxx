#include <testlib/testlib_test.h>
#include <brdb/brdb_value.h>
#include <vcl_iostream.h>

MAIN( test_value )
{

//////////////////////////////////////////////////////////////////
  brdb_value_t<double> double_val(2.3);
  brdb_value_t<bool> bool_val(true);  
  brdb_value_t<float> float_value(2.3f);
  brdb_value_t<long> long_value(1234l);
  brdb_value_t<int> int_value(-10);

//////////////////////////////////////////////////////////////////


  START ("DB Value");

  vcl_cout << "Registered Database Value Types:" << vcl_endl;
  vcl_map<vcl_string, const brdb_value*>::const_iterator itr = brdb_value::registry().begin();
  bool registration_test = true;
  for( ; itr != brdb_value::registry().end(); ++itr){
    vcl_cout << "   " << itr->first << vcl_endl;
    registration_test = (itr->first == itr->second->is_a()) && registration_test;
  }
    
  TEST("Registration", registration_test, true);

  brdb_value_t<float> float_val(2.3f);
  float f = float_val.value();
  TEST("value()", float_val, f);

  brdb_value& base_val = float_val;
  // MSVC++ 6 cannot handle this syntax
  // I will find a workaround later
  //float fb = base_val;
  //TEST("implicit conversion base", float_val, fb);
  
  TEST("type()", "float", brdb_value_t<float>::type());
  TEST("is_a()", (&base_val)->is_a(), brdb_value_t<float>::type());

  TEST("invalid assign()",base_val.assign(brdb_value_t<int>(10)), false);
  TEST("valid assign()",base_val.assign(brdb_value_t<float>(6.3f)), true);
  
  TEST("operator ==",base_val == brdb_value_t<float>(6.3f), true);
  TEST("operator !=",base_val != brdb_value_t<float>(6.4f), true);
  TEST("operator <",  base_val < brdb_value_t<float>(7.0f)  &&
                    !(brdb_value_t<float>(6.3f) < base_val) &&
                      brdb_value_t<float>(1.0f) < base_val  , true);
  TEST("operator <=", base_val <= brdb_value_t<float>(7.0f) &&
                      brdb_value_t<float>(6.3f) <= base_val &&
                      brdb_value_t<float>(1.0f) <= base_val , true);
  TEST("operator >",  base_val > brdb_value_t<float>(0.0f)  &&
                    !(brdb_value_t<float>(6.3f) > base_val) &&
                      brdb_value_t<float>(10.0f) > base_val  , true);
  TEST("operator >=", base_val >= brdb_value_t<float>(-2.1f) &&
                      brdb_value_t<float>(6.3f) >= base_val &&
                      brdb_value_t<float>(101.89f) >= base_val , true);

  SUMMARY();
}
