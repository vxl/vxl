// This is core/vgui/internals/vgui_simple_field.h
#ifndef vgui_simple_field_h_
#define vgui_simple_field_h_
//:
// \file
// \brief  Templated fields for dialog boxes.
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   23 Oct 99
//
//  Contains class  vgui_simple_field
//
// \verbatim
//  Modifications
//   23-OCT-1999  P.Pritchett - Initial version.
//   13-SEP-2002  K.Y.McGaul - Converted to doxygen style comments
// \endverbatim

#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "vgui_dialog_field.h"

//: Templated fields for dialog boxes.
//
// vgui_simple_field is templated over the type of variable it stores.
// The given template instances are all for fundamental types, but the
// template can be used for any data type that has the input and output
// stream operators defined (<< and >>)
template <class T>
class vgui_simple_field : public vgui_dialog_field
{
 public:
  vgui_simple_field(const char *text,T &variable_to_modify);
  ~vgui_simple_field();

  std::string current_value() const;
  bool update_value(const std::string &);

  T &var;
};

typedef vgui_simple_field<bool>   vgui_bool_field;
typedef vgui_simple_field<int>    vgui_int_field;
typedef vgui_simple_field<long>   vgui_long_field;
typedef vgui_simple_field<float>  vgui_float_field;
typedef vgui_simple_field<double> vgui_double_field;

#endif // vgui_simple_field_h_
