#ifndef xcv_threeview_manager_h_
#define xcv_threeview_manager_h_

//--------------------------------------------------------------------------------
// .NAME    xcv_threeview_manager
// .INCLUDE xcv/xcv_threeview_manager.h
// .FILE    xcv_threeview_manager.cxx
// .SECTION Description:
//
// .SECTION Author
//   K.Y.McGaul
// .SECTION Modifications:
//   K.Y.McGaul     05-MAY-2000    Initial version.
//--------------------------------------------------------------------------------

#include <mvl/TriTensor.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_rubberbander.h>
#include <vgui/vgui_easy2D.h>
#include "xcv_mview_manager.h"

class vgui_event;

class xcv_threeview_manager : public xcv_mview_manager
{
public:

  xcv_threeview_manager();
  ~xcv_threeview_manager();

  void set_tableau(vgui_tableau_sptr const& tab, unsigned tab_position);
  void set_tri_tensor(TriTensor* tt){tri_tensor = tt;}

  void toggle_tri_tensor_display();

  TriTensor* get_tri_tensor(){return tri_tensor;}

  void handle_tjunction_event(vgui_event const& e, vgui_tableau_sptr const& child_tab);

private:
  TriTensor* tri_tensor;
  vgui_tableau_sptr tabs[3];
  vgui_rubberbander_sptr rubberbands[3];
  vgui_easy2D_sptr easys[3];
  bool tri_tensor_is_displayed;

  void draw_tri_tensor(vgui_event const& e, vgui_tableau_sptr const& child_tab, bool make_permanent);
};

#endif // xcv_threeview_manager_
