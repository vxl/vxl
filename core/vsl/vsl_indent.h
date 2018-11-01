// This is core/vsl/vsl_indent.h
#ifndef vsl_indent_h_
#define vsl_indent_h_
//:
// \file
// \author Tim Cootes

#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Put indents into output streams, to produce more legible printed output
//  Its use is best described by example:
// \code
// std::cout<<vsl_indent()<<"No Indent\n";
// vsl_indent_inc(std::cout);
// std::cout<<vsl_indent()<<"1 Indent\n";
// vsl_indent_inc(std::cout);
// std::cout<<vsl_indent()<<"2 Indent\n";
// vsl_indent_dec(std::cout);
// std::cout<<vsl_indent()<<"1 Indent\n";
// vsl_indent_dec(std::cout);
// std::cout<<vsl_indent()<<"No Indent\n";
// \endcode
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
// \code
// class Fred
// {
//  public:
//   void print(std::ostream& os) const { os<<vsl_indent()<<"Fred's data"; }
// };
//
// std::ostream& operator<<(std::ostream& os, const Fred& fred)
// {
//   os<<"Fred:\n";
//   vsl_indent_inc(os);
//   fred.print(os);
//   vsl_indent_dec(os);
//   return os;
// }
//
// class Jim
// {
//  private:
//   Fred fred_;
//  public:
//   void print(std::ostream& os) const
//   {
//     os<<vsl_indent()<<fred_<<'\n'
//       <<vsl_indent()<<"Jim's other data\n";
//   }
// };
//
// std::ostream& operator<<(std::ostream& os, const Jim& jim)
// {
//   os<<"Jim:\n";
//   vsl_indent_inc(os);
//   jim.print(os);
//   vsl_indent_dec(os);
//   return os;
// }
//
// main()
// {
//   Jim jim;
//   std::cout<<jim<<std::endl;
// }
// \endcode
//
// This produces output:
// \verbatim
//  Jim:
//    Fred's data
//    Jim's other data
// \endverbatim

class vsl_indent
{
};

//: Increments current indent for given stream
void vsl_indent_inc(std::ostream& os);

//: Decrements current indent for given stream
void vsl_indent_dec(std::ostream& os);

//: Set number of spaces per increment step
void vsl_indent_set_tab(std::ostream& os,int);

//: Number of spaces per increment step
int vsl_indent_tab(std::ostream& os);

//: Set indentation to zero
void vsl_indent_clear(std::ostream& os);

//: Outputs current indent to os
std::ostream& operator<<(std::ostream& os, const vsl_indent& indent);

//: Tidy up the internal indent map to remove potential memory leaks
//  The details of indents for each stream are stored in a static
//  map.  When testing for memory leaks, this is flagged, creating
//  lots of noise in the output of memory leak checkers.
//  This call empties the map, removing the potential leak.
//  Pragmatically it is called in the vsl_delete_all_loaders()
void vsl_indent_clear_all_data();

#endif // vsl_indent_h_
