#ifndef vsl_indent_h_
#define vsl_indent_h_
#ifdef __GNUC__
#pragma interface
#endif

#include <vcl_iosfwd.h>

//: Put indents into output streams, to produce more legible printed output
//  It's use is best described by example:
// \verbatim
// vcl_cout<<vsl_indent()<<"No Indent"<<vcl_endl;
// vsl_inc_indent(vcl_cout);
// vcl_cout<<vsl_indent()<<"1 Indent"<<vcl_endl;
// vsl_inc_indent(vcl_cout);
// vcl_cout<<vsl_indent()<<"2 Indent"<<vcl_endl;
// vsl_dec_indent(vcl_cout);
// vcl_cout<<vsl_indent()<<"1 Indent"<<vcl_endl;
// vsl_dec_indent(vcl_cout);
// vcl_cout<<vsl_indent()<<"No Indent"<<vcl_endl;
// \endverbatim
//
// This produces output of the form
// \verbatim
// No Indent
//   1 Indent
//     2 Indent
//   1 Indent
// No Indent
// \endverbatim
//
// Example of use in class output:
// \verbatim
// class Fred
// {
// public:
//  void print(vcl_ostream& os) const { os<<vsl_indent(os)<<"Fred's data"; }
// };
//
// vcl_ostream& operator<<(vcl_ostream& os, const Fred& fred)
// {
//  os<<"Fred: "<<vcl_endl;
//  vsl_inc_indent(os);
//  fred.print(os);
//  vsl_dec_indent(os);
//  return os;
// }
//
//
// class Jim
// {
// private:
//  Fred fred_;
// public:
//  void print(vcl_ostream& os) const
//  {
//    os<<vsl_indent(os)<<fred_<<vcl_endl;
//    os<<vsl_indent(os)<<"Jim's other data"; }
//  };
//
// vcl_ostream& operator<<(vcl_ostream& os, const Jim& jim)
// {
//  os<<"Jim: "<<vcl_endl;
//  vsl_inc_indent(os);
//  jim.print(os);
//  vsl_dec_indent(os);
//  return os;
// }
//
// main()
// {
//  Jim jim;
//  vcl_cout<<jim<<vcl_endl;
// }
// \endverbatim
//
// This produces output:
// \verbatim
//  Jim:
//    Fred's data
//    Jim's other data
// \endverbatim
//
// \author Tim Cootes
class vsl_indent {
};

//: Increments current indent for given stream
void vsl_inc_indent(vcl_ostream& os);

//: Decrements current indent for given stream
void vsl_dec_indent(vcl_ostream& os);

//: Set number of spaces per increment step
void vsl_indent_set_tab(vcl_ostream& os,int);

//: Number of spaces per increment step
int vsl_indent_tab(vcl_ostream& os);

//: Set indentation to zero
void vsl_clear_indent(vcl_ostream& os);

//: Outputs current indent to os
vcl_ostream& operator<<(vcl_ostream& os, const vsl_indent& indent);

#endif // vsl_indent_h_