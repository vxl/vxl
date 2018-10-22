// This is core/vsl/vsl_stack_io.hxx
#ifndef vsl_stack_io_hxx_
#define vsl_stack_io_hxx_
//:
// \file
// \brief  binary IO functions for std::stack<T>
// \author K.Y.McGaul

#include <iostream>
#include "vsl_stack_io.h"
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_indent.h>

//====================================================================================
//: Write stack to binary stream
template <class T>
void vsl_b_write(vsl_b_ostream& s, const std::stack<T>& v)
{
  constexpr short version_no = 1;
  vsl_b_write(s, version_no);
  // Make a copy of v since we have to change a stack to get
  // the values out:
  std::stack<T> tmp_stack = v;

  unsigned int stack_size = (unsigned int)(v.size());
  vsl_b_write(s, stack_size);
  for (unsigned int i=0; i<stack_size; i++)
  {
    vsl_b_write(s,tmp_stack.top());
    tmp_stack.pop();
  }
}

//====================================================================================
//: Read stack from binary stream
template <class T>
void vsl_b_read(vsl_b_istream& is, std::stack<T>& v)
{
  if (!is) return;

  while (!v.empty()) v.pop(); // clear stack, which has no clear() member

  unsigned int stack_size;
  std::stack<T> tmp_stack;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
  case 1:
    vsl_b_read(is, stack_size);

    // We need to reverse the order of the values before we load them
    // back into the stack, so use another temporary stack for this:
    for (unsigned int i=0; i<stack_size; i++)
    {
      T tmp;
      vsl_b_read(is,tmp);
      tmp_stack.push(tmp);
    }
    for (unsigned int i=0; i<stack_size; i++)
    {
      v.push(tmp_stack.top());
      tmp_stack.pop();
    }
    break;
  default:
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, std::stack<T>&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

//====================================================================================
//: Output a human readable summary to the stream
template <class T>
void vsl_print_summary(std::ostream& os, const std::stack<T> &v)
{
  std::stack<T> tmp_stack = v;
  os << "Stack length: " << v.size() << '\n';

  unsigned int stack_size = (unsigned int)(v.size());
  for (unsigned int i=0; i<stack_size && i<5; i++)
  {
    os << vsl_indent() << ' ' << i << ": ";
    vsl_indent_inc(os);
    vsl_print_summary(os, tmp_stack.top());
    tmp_stack.pop();
    os << '\n';
    vsl_indent_dec(os);
  }
  if (stack_size > 5)
    os << " ...\n";
}


#define VSL_STACK_IO_INSTANTIATE(T) \
template void vsl_print_summary(std::ostream& s, const std::stack<T >& v); \
template void vsl_b_write(vsl_b_ostream& s, const std::stack<T >& v); \
template void vsl_b_read(vsl_b_istream& s, std::stack<T >& v)

#endif // vsl_stack_io_hxx_
