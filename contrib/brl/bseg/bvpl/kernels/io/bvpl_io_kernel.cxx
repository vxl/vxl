#include "bvpl_io_kernel.h"
//:
// \file
#include <vsl/vsl_binary_io.h>


//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & /*os*/, bvpl_kernel const & /*kernel*/)
{
  std::cerr << "Error: Trying to save but binary io not implemented\n";
  return;
}


//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & /*is*/, bvpl_kernel & /*kernel*/)
{
  std::cerr << "Error: Trying to save but binary io not implemented\n";
  return;
}

void vsl_print_summary(std::ostream & /*os*/, const bvpl_kernel & /*kernel*/)
{
  std::cerr << "Error: Trying to save but binary io not implemented\n";
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
    p = nullptr;
}

void vsl_b_write(vsl_b_ostream& os, const bvpl_kernel* &p)
{
  if (p==nullptr)
  {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else
  {
    vsl_b_write(os,true); // Indicate non-null pointer stored
    vsl_b_write(os,*p);
  }
}

void vsl_print_summary(std::ostream& os, const bvpl_kernel* &p)
{
  if (p==nullptr)
    os << "NULL PTR";
  else {
    os << "T: ";
    vsl_print_summary(os, *p);
  }
}


/******************bvpl_kernel_vector**************************/
//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & /*os*/, bvpl_kernel_vector const & /*kernel*/)
{
  std::cerr << "Error: Trying to save but binary io not implemented\n";
  return;
}


//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & /*is*/, bvpl_kernel_vector & /*kernel*/)
{
  std::cerr << "Error: Trying to save but binary io not implemented\n";
  return;
}

void vsl_print_summary(std::ostream & /*os*/, const bvpl_kernel_vector & /*kernel*/)
{
  std::cerr << "Error: Trying to save but binary io not implemented\n";
  return;
}

void vsl_b_read(vsl_b_istream& is,bvpl_kernel_vector* p)
{
  delete p;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    p = new bvpl_kernel_vector();
    vsl_b_read(is, *p);
  }
  else
    p = nullptr;
}

void vsl_b_write(vsl_b_ostream& os, const bvpl_kernel_vector* &p)
{
  if (p==nullptr)
  {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else
  {
    vsl_b_write(os,true); // Indicate non-null pointer stored
    vsl_b_write(os,*p);
  }
}

void vsl_print_summary(std::ostream& os, const bvpl_kernel_vector* &p)
{
  if (p==nullptr)
    os << "NULL PTR";
  else {
    os << "T: ";
    vsl_print_summary(os, *p);
  }
}
