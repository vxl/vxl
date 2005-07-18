// This is mul/mbl/mbl_test.h
#ifndef mbl_test_h_
#define mbl_test_h_
//:
// \file
// \brief A place for useful things associated with testing.
// \author iscott
// \date  Dec 2003

#include <vcl_sstream.h>
#include <vcl_string.h>

//: Test if the summaries of two objects are the same.
// Both objects \a a and \a b must be the same class, and have
// vsl_print_summary defined for them.
// \param exceptions is an optional, 0 terminated, list of c-strings.
//
// Any pair of lines from the two summaries that don't match
// each other, but do contain one of the exceptions strings will
// be ignored.
// \code
//  base_class_wibble *base_ptr, *base_ptr2;
//  ...
//  // Compare but ignore all lines containing the string "Model address:" :
//  const char *exceptions[] = { "Model address:", 0}; // Don't forget terminating 0
//
//  TEST("saved and loaded extractors are identical",
//       mbl_test_summaries_are_equal(base_ptr,base_ptr2, exceptions),
//       true);
// \endcode

template <class S>
bool mbl_test_summaries_are_equal(const S &a, const S &b, const char **exceptions=0 )
{
  vcl_stringstream ssa, ssb;
  vcl_string sa, sb;
  vsl_print_summary(ssa, a);
  vsl_print_summary(ssb, b);
  while (!ssa.eof() || !ssb.eof())
  {
    vcl_getline(ssa, sa);
    vcl_getline(ssb, sb);
    if (sa != sb && exceptions == 0)
      return false;
    else if (sa != sb)
    {
      bool exception_found = false;
      for (const char **it = exceptions; *it!=0; ++it)
        if (sa.find(*it)!=vcl_string::npos && sb.find(*it)!=vcl_string::npos)
          exception_found = true;
      if (!exception_found) return false;
    }
  }
  return true;
}


//: A historical measurement recording framework.
// Currently the function will append the measurement to the file specified
// by ${MBL_TEST_SAVE_MEASUREMENT_PATH}/measurement_path.
// In the longer term it may save the value via Dart2.
void mbl_test_save_measurement( const vcl_string &measurement_path, double value);


#endif // mbl_test_h_
