#ifndef vbl_io_test_classes_cxx_
#define vbl_io_test_classes_cxx_

#include "vbl_io_test_classes.h"
#include <vbl/vbl_smart_ptr.h>
#include <vbl/io/vbl_io_smart_ptr.h>

//:
// \file
// \brief Contains some stuff for testing smart ptr IO

#include <vbl/vbl_smart_ptr.txx>
VBL_SMART_PTR_INSTANTIATE(impl);

#include <vbl/io/vbl_io_smart_ptr.txx>
VBL_IO_SMART_PTR_INSTANTIATE(impl);


typedef vbl_smart_ptr<impl> base_sptr;

// Remember, the template instances must be in
// Templates, but they use these classes, so the class
// implementation must be in the library even if only
// one test uses it.

int impl::reftotal = 0;

impl::impl(int nn) : n(nn) {
  reftotal++;
  vcl_cout <<  "impl ctor : this=" << (void*)this << vcl_endl;
}

impl::impl() : n(7) {
  reftotal++;
  vcl_cout <<  "impl ctor : this=" << (void*)this << vcl_endl;
}

impl::~impl() {
  reftotal--;
  vcl_cout <<  "impl dtor : this=" << (void*)this << vcl_endl;
}

void impl::Print (vcl_ostream &str) {
  str << "impl(" << n << ") ";
}

void impl::checkcount () {
  if (reftotal == 0)
    vcl_cout << "impl : PASSED\n";
  else
    vcl_cout << "impl : FAILED : count = " << reftotal << vcl_endl;
}

void vsl_b_write(vsl_b_ostream& os, const impl &p)
{
  // write version number
  const short io_version_no = 1;
  vsl_b_write(os, io_version_no);

  vsl_b_write(os, p.n);
}

void vsl_b_read(vsl_b_istream& is, impl &p)
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
   case 1:
    vsl_b_read(is, p.n);
    break;
   default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, impl&)\n";
    vcl_cerr << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

void vsl_print_summary(vcl_ostream& os, const impl &p)
{
  os << p.n;
}

void vsl_b_read(vsl_b_istream& is, impl * &p)
{
  delete p;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    p = new impl();
    vsl_b_read(is, *p);
  }
  else
    p = 0;
}

void vsl_b_write(vsl_b_ostream& os, const impl *p)
{
  if (p==0)
  {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else
  {
    vsl_b_write(os,true); // Indicate non-null pointer stored
    vsl_b_write(os,*p);
  }
}

void vsl_print_summary(vcl_ostream& os, const impl *p)
{
  if (p==0)
    os << "NULL PTR";
  else
  {
    os << "T" << ": ";
    vsl_print_summary(os, *p);
  }
};

#endif // vbl_io_test_classes_cxx_
