#include "bvpl_io_kernel.h"

#include <vsl/vsl_binary_io.h>



//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & os, bvpl_kernel const &kernel)

{
  vcl_cerr << "Error: Trying to save but binary io not implemented \n";
  return;
}


//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & is, bvpl_kernel &kernel)

{
  vcl_cerr << "Error: Trying to save but binary io not implemented \n";
  return;
}

void vsl_print_summary(vcl_ostream &os, const bvpl_kernel &kernel)

{
  vcl_cerr << "Error: Trying to save but binary io not implemented \n";
  return;
}

void vsl_b_read(vsl_b_istream& is,bvpl_kernel* p)
{
  delete p;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    p = new bvpl_kernel();
    vsl_b_read(is, *p);
  }
  else
    p = 0;
}

void vsl_b_write(vsl_b_ostream& os, const bvpl_kernel* &p)
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

void vsl_print_summary(vcl_ostream& os, const bvpl_kernel* &p)
{
  if (p==0)
    os << "NULL PTR";
  else {
    os << "T: ";
    vsl_print_summary(os, *p);
  }
}

void vsl_b_read(vsl_b_istream& is,bvpl_kernel_sptr p)
{
  delete p.ptr();
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    p = new bvpl_kernel();
    vsl_b_read(is, p.ptr());
  }
  else
    p = 0;
}

void vsl_b_write(vsl_b_ostream& os, const bvpl_kernel_sptr &p)
{
  if (p==0)
  {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else
  {
    vsl_b_write(os,true); // Indicate non-null pointer stored
    vsl_b_write(os,p.ptr());
  }
}

void vsl_print_summary(vcl_ostream& os, const bvpl_kernel_sptr &p)
{
  if (p==0)
    os << "NULL PTR";
  else {
    os << "T: ";
    vsl_print_summary(os, p.ptr());
  }
}