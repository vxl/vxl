// This is core/vgui/internals/vgui_string_field.h
#ifndef vgui_string_field_h_
#define vgui_string_field_h_
//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   24 Oct 99
// \brief  A dialog field implementation that holds a std::string.
//
// \verbatim
//  Modifications
//   24-OCT-1999 P.Pritchett - Initial version.
//   11-SEP-2002 K.Y.McGaul - Changed to Doxygen style comments.
//   10-SEP-2004 Peter Vanroose  Inlined all 1-line methods in class decl
// \endverbatim

#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "vgui_dialog_field.h"

//: A dialog field implementation that holds a std::string.
class vgui_string_field : public vgui_dialog_field
{
  std::string &var_;

 public:
  //: Constructor - creates a string field with the given text as label.
  vgui_string_field(const char* txt, std::string& var_to_modify)
  : vgui_dialog_field(txt) , var_(var_to_modify) {}

  //: Destructor.
  ~vgui_string_field() {}

  //: Returns the current value of the string field.
  std::string current_value() const { return var_; }

  //: Set the given string to be the value of the string field.
  bool update_value(std::string const& s) { var_ = s; return true; }
};

#endif // vgui_string_field_h_
