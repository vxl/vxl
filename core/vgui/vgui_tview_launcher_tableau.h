// This is core/vgui/vgui_tview_launcher_tableau.h
#ifndef vgui_tview_launcher_tableau_h_
#define vgui_tview_launcher_tableau_h_
//:
// \file
// \brief  Tableau which pops up tableau tree (tview) on Shift-G
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   21 Oct 99
//
// Contains classes  vgui_tview_launcher_tableau  vgui_tview_launcher_tableau_new
//
// \verbatim
//  Modifications
//   21-OCT-1999 P.Pritchett - Initial version.
//   10-AUG-2002 K.Y.McGaul - Converted to Doxygen style comments.
//   01-OCT-2002 K.Y.McGaul - Moved tview_launcher to tview_launcher_tableau.
// \endverbatim


#include "vgui_tview_launcher_tableau_sptr.h"
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_event_condition.h>


//: Tableau which pops up tableau tree (tview) on Shift-G
//
// vgui_tview_launcher_tableau responds to the keypress "Shift-G", and pops up a picture
// of the tableau graph of the adaptor under the mouse pointer.  It is generally
// placed in a composite to provide debugging facilities, and is provided by
// default if you use the vgui_shell_tableau.
class vgui_tview_launcher_tableau : public vgui_tableau
{
 public:
  //: Constructor - don't use this, use vgui_tview_launcher_tableau_new.
  vgui_tview_launcher_tableau();

  //: Handle all events sent to this tableau.
  //  In particular, use Shift-G events to pop up a tableau tree.
  bool handle(const vgui_event&);

  //: Returns the type of this tableau ('vgui_tview_launcher_tableau').
  std::string type_name() const;

  //:
  void get_popup(const vgui_popup_params&, vgui_menu& menu);

  //:
  void go(vgui_adaptor*);

  //:
  vgui_event_condition c_graph;

 protected:
  //: Destructor - called by vgui_tview_launcher_tableau_sptr.
  ~vgui_tview_launcher_tableau() { }
};

//: Creates a smart-pointer to a vgui_tview_launcher_tableau tableau.
struct vgui_tview_launcher_tableau_new : public vgui_tview_launcher_tableau_sptr
{
  typedef vgui_tview_launcher_tableau_sptr base;

  //: Constructor - creates a vgui_tview_launcher_tableau.
  vgui_tview_launcher_tableau_new() : base(new vgui_tview_launcher_tableau) { }
};

#endif // vgui_tview_launcher_tableau_h_
