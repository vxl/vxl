// This is oxl/vgui/impl/Fl/vgui_Fl_dialog_impl.h
#ifndef vgui_Fl_dialog_impl_h_
#define vgui_Fl_dialog_impl_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   25 Oct 99
// \brief  The FLTK implemenation of vgui_dialog_impl.
//
// \verbatim
//  Modifications
//   K.Y.McGaul  25-JAN-00  Moved all field functions to vgui_dialog_impl.
//                          Added _widget functions.
//   K.Y.McGaul  17-SEP-2002 - Changed to doxygen style comments.
// \endverbatim

#include <vgui/internals/vgui_dialog_impl.h>

//: The FLTK implemenation of vgui_dialog_impl.
class vgui_Fl_dialog_impl : public vgui_dialog_impl
{
 public:
  vgui_Fl_dialog_impl(const char* name);
  ~vgui_Fl_dialog_impl();

  void* bool_field_widget(const char*, bool&);
  void* int_field_widget(const char*, int&);
  void* long_field_widget(const char*, long&);
  void* float_field_widget(const char*, float&);
  void* double_field_widget(const char*, double&);
  void* string_field_widget(const char*, vcl_string&);
  void* choice_field_widget(const char*, const vcl_vector<vcl_string>&, int&);

  bool ask();
};

#endif // vgui_Fl_dialog_impl_h_
