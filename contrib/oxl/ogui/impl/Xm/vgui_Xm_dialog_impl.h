// This is oxl/ogui/impl/Xm/vgui_Xm_dialog_impl.h
#ifndef vgui_Xm_dialog_impl_h_
#define vgui_Xm_dialog_impl_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author
//              Philip C. Pritchett, 25 Oct 99
//              Robotics Research Group, University of Oxford
//
// \verbatim
// Modifications
//   K.Y.McGaul  25-JAN-00  Moved all field functions to vgui_dialog_impl.
//                          Added choice_field_widget function.
// \endverbatim
//-----------------------------------------------------------------------------

#include <vgui/internals/vgui_dialog_impl.h>
#include <X11/IntrinsicP.h>

class vgui_Xm_dialog_impl : public vgui_dialog_impl
{
 public:
  vgui_Xm_dialog_impl(const char* name);
  ~vgui_Xm_dialog_impl();

  void* choice_field_widget(const char*, const vcl_vector<vcl_string>&, int&);

  bool ask();

  Widget dialog_shell;
  Widget dialog_form;
};

#endif // vgui_Xm_dialog_impl_h_
