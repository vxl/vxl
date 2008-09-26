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
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vul/vul_reg_exp.h>

//: Test if the summaries of two objects are the same.
// Both objects \a a and \a b must be the same class, and have
// vsl_print_summary defined for them.
// \param exceptions is an optional, 0 terminated, list of c-strings.
//
// Any pair of lines from the two summaries that don't match
// each other, but do match one of the exception regular expressions will
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
  vcl_vector<vul_reg_exp> exceptions_re;
  while (exceptions && *exceptions)
  {
    exceptions_re.push_back(vul_reg_exp(*exceptions));
    exceptions++;
  }

  while (!ssa.eof() || !ssb.eof())
  {
    vcl_getline(ssa, sa);
    vcl_getline(ssb, sb);
    if (sa != sb && exceptions_re.empty())
    {
      vcl_cerr << "Found differences:\n>"<<sa<<"\n<"<<sb<<vcl_endl;
      return false;
    }
    else if (sa != sb)
    {
      bool exception_found = false;
//      for (const char **it = exceptions; *it!=0; ++it)
//        if (sa.find(*it)!=vcl_string::npos && sb.find(*it)!=vcl_string::npos)
      for (vcl_vector<vul_reg_exp>::iterator it=exceptions_re.begin(), end=exceptions_re.end();
        it != end; ++it)
        if (it->find(sa) && it->find(sb))
        {
          exception_found = true;
          break;
        }
      if (!exception_found)
      {
        vcl_cerr << "Found differences:\n>"<<sa<<"\n<"<<sb<<vcl_endl;
        return false;
      }
    }
  }
  return true;
}


//: A historical measurement recording framework.
// The function will append the measurement to the file specified
// by $CMAKE_VAR{VXL_MBL_TEST_SAVE_MEASUREMENT_PATH}/measurement_path. The
// value is also copied to stdout where it can be automatically
// picked up by CTest and Dart.
// We suggest formatting the measurement path as follows
// "path/from/code/root/to/test_source_filename_minus_extension/MEASUREMENT_DESCRIPTION",
// e.g. in the file "$CODE_SRC/algo/krr/tests/test_krr_optimise_model_parameters.cxx" 
// \verbatim
// mbl_test_save_measurement("algo/krr/tests/test_krr_optimise_model_parameters/Point_To_Point_RMS_Error",value);
// \endverbatim
void mbl_test_save_measurement( const vcl_string &measurement_path, double value);


#endif // mbl_test_h_
