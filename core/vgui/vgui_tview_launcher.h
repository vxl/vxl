// This is oxl/vgui/vgui_tview_launcher.h
#ifndef vgui_tview_launcher_h_
#define vgui_tview_launcher_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//
// .NAME vgui_tview_launcher - Pop up tableau tree (tview) on Shift-G
// .LIBRARY vgui
// .HEADER vxl Package
// .INCLUDE vgui/vgui_tview_launcher.h
// .FILE vgui_tview_launcher.cxx
//
// .SECTION Description
//
// vgui_tview_launcher responds to the keypress "Shift-G", and pops up a picture
// of the tableau graph of the adaptor under the mouse pointer.  It is generally
// placed in a composite to provide debugging facilities, and is provided by default
// if you use the vgui_shell_tableau.
//
// .SECTION Author
//              Philip C. Pritchett, 21 Oct 99
//              Robotics Research Group, University of Oxford
//
// .SECTION Modifications
//
//-----------------------------------------------------------------------------

#include "vgui_tview_launcher_sptr.h"
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_event_condition.h>

class vgui_tview_launcher : public vgui_tableau
{
 public:
  vgui_tview_launcher();
  bool handle(const vgui_event&);

  vcl_string type_name() const;

  void get_popup(const vgui_popup_params&, vgui_menu& menu);

  void go(vgui_adaptor*);

  vgui_event_condition c_graph;

 protected:
  ~vgui_tview_launcher() { }
};

struct vgui_tview_launcher_new : public vgui_tview_launcher_sptr
{
  typedef vgui_tview_launcher_sptr base;
  vgui_tview_launcher_new() : base(new vgui_tview_launcher) { }
};

#endif // vgui_tview_launcher_h_
