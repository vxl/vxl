// This is oxl/vgui/internals/vgui_simple_field.h
#ifndef vgui_simple_field_h_
#define vgui_simple_field_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//
// .NAME vgui_simple_field - templated dialog fields
// .LIBRARY vgui
// .HEADER vxl Package
// .INCLUDE vgui/internals/vgui_simple_field.h
// .FILE internals/vgui_simple_field.cxx
//
// .SECTION Description:
//
// vgui_simple_field is templated over the type of variable it stores.
// The given template instances are all for fundamental types, but the
// template can be used for any data type that has the input and output
// stream operators defined (<< and >>)
//
// .SECTION Author:
//              Philip C. Pritchett, 23 Oct 99
//              Robotics Research Group, University of Oxford
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <vcl_string.h>
#include "vgui_dialog_field.h"

template <class T>
class vgui_simple_field : public vgui_dialog_field
{
 public:

  vgui_simple_field(const char *text,T &variable_to_modify);
  ~vgui_simple_field();

  vcl_string current_value() const;
  bool update_value(const vcl_string &);

  T &var;
};

typedef vgui_simple_field<bool> vgui_bool_field;
typedef vgui_simple_field<int> vgui_int_field;
typedef vgui_simple_field<long> vgui_long_field;
typedef vgui_simple_field<float> vgui_float_field;
typedef vgui_simple_field<double> vgui_double_field;

#endif // vgui_simple_field_h_
