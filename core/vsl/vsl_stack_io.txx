// This is vxl/vsl/vsl_stack_io.txx

//:
// \file   
// \brief  binary IO functions for vcl_stack<T>
// \author K.Y.McGaul
// Implementation

#include <vsl/vsl_stack_io.h>
#include <vsl/vsl_binary_io.h>

//====================================================================================
//: Write stack to binary stream
template <class T>
void vsl_b_write(vsl_b_ostream& s, const vcl_stack<T>& v)
{
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
void vsl_b_read(vsl_b_istream& s, vcl_stack<T>& v)
{
  unsigned stack_size;
  vsl_b_read(s, stack_size);

  // We need to reverse the order of the values before we load them
  // back into the stack, so use another temporary stack for this:
  vcl_stack<T> tmp_stack;
  for (unsigned i=0; i<stack_size; i++)
  {
    T tmp;
    vsl_b_read(s,tmp);
    tmp_stack.push(tmp);
  }
  for (unsigned i=0; i<stack_size; i++)
  {
    v.push(tmp_stack.top());
    tmp_stack.pop();
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
  for (unsigned i=0; i<stack_size && i<5 ; i++)
  {
    os << " " << i << ": ";
    vsl_print_summary(os, tmp_stack.top());
    tmp_stack.pop();
    os << vcl_endl;
  }
  if (stack_size > 5)
    os << " ..." << vcl_endl;
}


#define VSL_STACK_INSTANTIATE(T) \
template void vsl_print_summary(vcl_ostream& s, const vcl_stack<T >& v); \
template void vsl_b_write(vsl_b_ostream& s, const vcl_stack<T >& v); \
template void vsl_b_read(vsl_b_istream& s, vcl_stack<T >& v); \
;
