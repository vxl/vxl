// This is mul/mbl/mbl_test.h
#ifndef mbl_test_h_
#define mbl_test_h_
//:
// \file
// \brief A place for useful things associated with testing.
// \author iscott
// \date  Dec 2003

//: Test if the summaries of two objects are the same
// Both objects \a a and \b must be the same class, and have
// vsl_print_summary defined for them.
// \param exceptions is an optional 0 terminated list of c-strings.
// Any pair of lines from the two summaries that don't match
// each other, but do contain one of the exceptions strings will
// be ignored.
// \vebatim
//  base_class_wibble * base_ptr, base_ptr2;
//  ...
//  const char *exceptions[] = { "Model address:", 0}; // Don't forget terminating 0
//
//  TEST("saved and loaded extractors are identical",
//       mbl_test_summaries_are_equal(base_ptr,base_ptr2, exceptions),
//       true);
// \endverbatim
template <class S>
bool mbl_test_summaries_are_equal(const S &a, const S &b, const char **exceptions=0 )
{
  vcl_stringstream ssa, ssb;
  vcl_string sa, sb;
  vsl_print_summary(ssa, a);
  vsl_print_summary(ssb, b);
  static const vcl_string::size_type npos = vcl_string::npos;
  while (!ssa.eof() || !ssb.eof())
  {
    vcl_getline(ssa, sa);
    vcl_getline(ssb, sb);
    if (sa != sb && exceptions != 0)
    {
      bool exception_found = false;
      {
        for (const char **it = exceptions; *it!=0; ++it)
          if (sa.find(*it)!=npos && sb.find(*it)!=npos) 
            exception_found = true;
      }
      if (!exception_found) return false;
    }
  }
  return true;
}



#endif // mbl_test_h_
