// This is core/vsl/vsl_stack_io.txx
#ifndef vsl_stack_io_txx_
#define vsl_stack_io_txx_
//:
// \file
// \brief  binary IO functions for vcl_stack<T>
// \author K.Y.McGaul

#include "vsl_stack_io.h"
#include <vsl/vsl_binary_io.h>

//====================================================================================
//: Write stack to binary stream
template <class T>
void vsl_b_write(vsl_b_ostream& s, const vcl_stack<T>& v)
{
  const short version_no = 1;
  vsl_b_write(s, version_no);
  // Make a copy of v since we have to change a stack to get
  // the values out:
  vcl_stack<T> tmp_stack = v;

  unsigned stack_size = v.size();
  vsl_b_write(s, stack_size);
  for (unsigned i=0; i<stack_size; i++)
  {
    vsl_b_write(s,tmp_stack.top());
    tmp_stack.pop();
  }
}

//====================================================================================
//: Read stack from binary stream
template <class T>
void vsl_b_read(vsl_b_istream& is, vcl_stack<T>& v)
{
  if (!is) return;

  while (!v.empty()) v.pop(); // clear stack, which has no clear() member

  unsigned stack_size;
  vcl_stack<T> tmp_stack;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
  case 1:
    vsl_b_read(is, stack_size);

    // We need to reverse the order of the values before we load them
    // back into the stack, so use another temporary stack for this:
    for (unsigned i=0; i<stack_size; i++)
    {
      T tmp;
      vsl_b_read(is,tmp);
      tmp_stack.push(tmp);
    }
    for (unsigned i=0; i<stack_size; i++)
    {
      v.push(tmp_stack.top());
      tmp_stack.pop();
    }
    break;
  default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vcl_stack<T>&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

//====================================================================================
//: Output a human readable summary to the stream
template <class T>
void vsl_print_summary(vcl_ostream& os, const vcl_stack<T> &v)
{
  vcl_stack<T> tmp_stack = v;
  os << "Stack length: " << v.size() << vcl_endl;

  unsigned stack_size = v.size();
  for (unsigned i=0; i<stack_size && i<5; i++)
  {
    os << ' ' << i << ": ";
    vsl_print_summary(os, tmp_stack.top());
    tmp_stack.pop();
    os << vcl_endl;
  }
  if (stack_size > 5)
    os << " ...\n";
}


#define VSL_STACK_IO_INSTANTIATE(T) \
template void vsl_print_summary(vcl_ostream& s, const vcl_stack<T >& v); \
template void vsl_b_write(vsl_b_ostream& s, const vcl_stack<T >& v); \
template void vsl_b_read(vsl_b_istream& s, vcl_stack<T >& v)

#endif // vsl_stack_io_txx_
