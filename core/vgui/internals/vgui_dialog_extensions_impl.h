// This is core/vgui/internals/vgui_dialog_extensions_impl.h
#ifndef vgui_dialog_extensions_impl_h_
#define vgui_dialog_extensions_impl_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief  vgui_dialog_extensions_impl is the abstract base class for dialog_extensions implementation.
// \author Gamze Tunali, LEMS, Brown University
// \date   16 Nov 2007
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_string.h>
#include "vgui_dialog_impl.h"

//: vgui_dialog_extensions_impl is the abstract base class for dialog extensions implementation.
//
//  It contains methods for adding fields corresponding to those in
//  vgui_dialog_extensions. It has directory selection browser and line
//  break in addition to the vgui_dialog_impl capabilities (parent class)
class vgui_dialog_extensions_impl : public vgui_dialog_impl
{
 public:
  //: Constructor - create an empty dialog with the given title.
  vgui_dialog_extensions_impl(const char* dialog_name);

  //: Destructor - delete this dialog box.
  virtual ~vgui_dialog_extensions_impl();

  //: Add a popup directory browser to the dialog box.
  void dir_browser(const char*, vcl_string&, vcl_string&);

  //: Add a line break to the dialog box
  void line_break();

  //: Pointer to a GUI widget for a directory browser.
  virtual void* dir_browser_widget(const char*, vcl_string&, vcl_string&);

  //: Display the dialog box with user added line breaks and a layout and collect data from the user.
  // Dialog elements are added next to each other until a line break element is found.
  virtual bool ask() = 0;
};

#endif // vgui_dialog_extensions_impl_h_
