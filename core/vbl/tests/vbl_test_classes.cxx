#include "vbl_test_classes.h"
#include <vcl_iostream.h>

// Remember, the template instances must be in
// Templates, but they use these classes, so the class
// implementation must be in the library even if only
// one test uses it.

int base_impl::reftotal = 0;

base_impl::base_impl(int nn) : n(nn)
{
  reftotal++;
  vcl_cout <<  "base_impl ctor : this=" << (void*)this << vcl_endl;
}

base_impl::base_impl() : n(7)
{
  reftotal++;
  vcl_cout <<  "base_impl ctor : this=" << (void*)this << vcl_endl;
}

base_impl::~base_impl()
{
  reftotal--;
  vcl_cout <<  "base_impl dtor : this=" << (void*)this << vcl_endl;
}

void base_impl::Print (vcl_ostream &str)
{
  str << "base_impl(" << n << ") ";
}

bool base_impl::checkcount ( int count )
{
  if (reftotal == count) {
    vcl_cout << "base_impl : PASSED" << vcl_endl;
    return true;
  } else {
    vcl_cout << "base_impl : FAILED : count = " << reftotal
             << " (expecting " << count << ')' << vcl_endl;
    return false;
  }
}

#include <vbl/vbl_smart_ptr.txx>

VBL_SMART_PTR_INSTANTIATE(base_impl);
VBL_SMART_PTR_INSTANTIATE(derived_impl);

#include <vcl_list.txx>

VCL_LIST_INSTANTIATE(base_sptr);

#include <vcl_algorithm.txx>

VCL_OPERATOR_NE_INSTANTIATE(base_sptr);
VCL_CONTAINABLE_INSTANTIATE(base_sptr);
