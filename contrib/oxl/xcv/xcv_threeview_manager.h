#ifndef xcv_threeview_manager_h_
#define xcv_threeview_manager_h_
//
// This is xcv/xcv_threeview_manager.h
//
//: Handles events which occur in one view but are displayed in three views
//  simultaneously (eg. displaying the epipolar lines in two view corresponding
//  to a mouse press in the other view.)
//  Also holds data which links three views, eg. TriTensor
//
// \file
// \author  K.Y.McGaul
// \verbatim
//  Modifications:
//    K.Y.McGaul     05-MAY-2000    Initial version.
// \endverbatim
//

#include <mvl/TriTensor.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_rubberbander.h>
#include <vgui/vgui_easy2D.h>
#include "xcv_mview_manager.h"

class vgui_event;

class xcv_threeview_manager : public xcv_mview_manager
{
public:

  //: Constructor.
  xcv_threeview_manager();
  //: Destructor.
  ~xcv_threeview_manager();
  
  //: Set the tableau at the given position to the given tableau.
  void set_tableau(vgui_tableau_sptr const& tab, unsigned tab_position);
  //: Set TriTensor to given value.
  void set_tri_tensor(TriTensor* tt){tri_tensor = tt;}

  //: Toggle between displaying and not displaying the TriTensor.
  void toggle_tri_tensor_display();

  //: Return the tri-tensor held by this manager.
  TriTensor* get_tri_tensor(){return tri_tensor;}

  //: Handle all events sent to this manager.
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
