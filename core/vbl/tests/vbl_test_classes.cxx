#include <iostream>
#include "vbl_test_classes.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// Remember, the template instances must be in
// Templates, but they use these classes, so the class
// implementation must be in the library even if only
// one test uses it.

int base_impl::reftotal = 0;

base_impl::base_impl(int nn) : n(nn)
{
  reftotal++;
  std::cout <<  "base_impl ctor : this=" << (void*)this << std::endl;
}

base_impl::base_impl() : n(7)
{
  reftotal++;
  std::cout <<  "base_impl ctor : this=" << (void*)this << std::endl;
}

base_impl::~base_impl()
{
  reftotal--;
  std::cout <<  "base_impl dtor : this=" << (void*)this << std::endl;
}

void base_impl::Print (std::ostream &str)
{
  str << "base_impl(" << n << ") ";
}

bool base_impl::checkcount ( int count )
{
  if (reftotal == count) {
    std::cout << "base_impl : PASSED" << std::endl;
    return true;
  } else {
    std::cout << "base_impl : FAILED : count = " << reftotal
             << " (expecting " << count << ')' << std::endl;
    return false;
  }
}

#include <vbl/vbl_smart_ptr.hxx>

VBL_SMART_PTR_INSTANTIATE(base_impl);
VBL_SMART_PTR_INSTANTIATE(derived_impl);
