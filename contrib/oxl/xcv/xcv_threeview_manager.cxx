// This is oxl/xcv/xcv_threeview_manager.cxx
#include "xcv_threeview_manager.h"
//:
//  \file
// See xcv_threeview_manager.h for a description of this file.
//
// \author K.Y.McGaul

#include <vcl_iostream.h>
#include <vgui/vgui_macro.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_find.h>

//static bool debug = true;
static bool dragging = false;

//-----------------------------------------------------------------------------
xcv_threeview_manager::xcv_threeview_manager()
  : tri_tensor(0)
  , tri_tensor_is_displayed(true)
{ }

//-----------------------------------------------------------------------------
xcv_threeview_manager::~xcv_threeview_manager() { }

//-----------------------------------------------------------------------------
//: Set the tableau at the given position to the given tableau.
//-----------------------------------------------------------------------------
void xcv_threeview_manager::set_tableau(vgui_tableau_sptr const& tab, unsigned tab_nb)
{
  if (tab_nb > 2)
  {
    vgui_macro_warning << "Tableau position [" << tab_nb << "] out of range\n";
    return;
  }
  tabs[tab_nb] = tab;
  rubberbands[tab_nb].vertical_cast(vgui_find_below_by_type_name(tab, 
    vcl_string("vgui_rubberband_tableau")));
  if (! rubberbands[tab_nb])
    vgui_macro_warning << "Unable to find rubberbander for tableau1\n";
  easys[tab_nb].vertical_cast(vgui_find_below_by_type_name(tab, 
    vcl_string("vgui_easy2D_tableau")));
  if (! easys[tab_nb])
    vgui_macro_warning << "Unable to find easy2D for tableau" << tab_nb << '\n';
}

//-----------------------------------------------------------------------------
//: Toggle between displaying and not displaying the TriTensor
//-----------------------------------------------------------------------------
void xcv_threeview_manager::toggle_tri_tensor_display()
{
  if (tri_tensor_is_displayed == false)
    tri_tensor_is_displayed = true;
  else
    tri_tensor_is_displayed = false;
}

//-----------------------------------------------------------------------------
//:
//-----------------------------------------------------------------------------
void xcv_threeview_manager::draw_tri_tensor(vgui_event const& e, vgui_tableau_sptr const& child_tab,
                                            bool /* make_permanent */)
{
  // Get the address of the event and turn it into a HomgPoint2D:
  vgui_projection_inspector p_insp;
  float ix, iy;
  p_insp.window_to_image_coordinates(e.wx, e.wy, ix, iy);

  if (child_tab == tabs[0])
  {
  }
  else if (child_tab == tabs[1])
  {
  }
  else if (child_tab == tabs[2])
  {
  }
  else
  {
    vgui_macro_warning << "Unknown child tableau: " << child_tab << '\n';
    return;
  }
  // FIXME - to be completed
  vgui_macro_warning << "xcv_threeview_manager::draw_tri_tensor() not yet fully implemented\n";
}


//-----------------------------------------------------------------------------
//: Handle all events sent to this manager.
//-----------------------------------------------------------------------------
void xcv_threeview_manager::handle_tjunction_event(vgui_event const& e, vgui_tableau_sptr const& child_tab)
{
  if (e.type == vgui_BUTTON_DOWN && e.modifier == vgui_MODIFIER_NULL && e.button == vgui_LEFT)
    dragging = true;
  if (e.type == vgui_BUTTON_UP || e.type == vgui_LEAVE)
  {
    dragging = false;
    rubberbands[0]->post_redraw();
  }
  if (e.type == vgui_MOTION && dragging == true)
  {
    if (tri_tensor != 0 && tri_tensor_is_displayed)
    {
      draw_tri_tensor(e, child_tab, false);
    }
  }

  if (e.type == vgui_BUTTON_DOWN && e.modifier == vgui_MODIFIER_NULL && e.button == vgui_MIDDLE)
  {
    if (tri_tensor != 0 && tri_tensor_is_displayed)
    {
      draw_tri_tensor(e, child_tab, true);
    }
  }
}
