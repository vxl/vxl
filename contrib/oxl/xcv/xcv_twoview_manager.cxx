// This is oxl/xcv/xcv_twoview_manager.cxx
#include "xcv_twoview_manager.h"
//:
// \file
// \author  K.Y.McGaul
// \brief See xcv_twoview_manager.h for a description of this file.
//

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_rubberband_tableau.h>
#include <vgui/vgui_macro.h>
#include <vgui/vgui_text_graph.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_find.h>
#include <mvl/HomgInterestPointSet.h>
#include <mvl/HomgInterestPoint.h>

//-----------------------------------------------------------------------------
//: Constructor.
//-----------------------------------------------------------------------------
xcv_twoview_manager::xcv_twoview_manager()
  : f_matrix(nullptr)
  , h_matrix(nullptr)
  , corner_matches(nullptr)
  , f_matrix_is_displayed(true)
  , h_matrix_is_displayed(true)
  , corner_matches_are_displayed(false)
  , transfer_index(0)
  , dragging(false)
{
#ifdef HAS_MFC
  // Until somebody implements overlays for mfc, this
  // is the default for windows (see also vgui_rubberbander).
  use_overlays = false;
#else
  // This is the default on non-windows platforms.
  // Please keep it that way or document clearly why
  // it must be changed. It is the default because
  // Mesa (or glx) is too slow otherwise.
  use_overlays = true;
#endif
}

//-----------------------------------------------------------------------------
//: Destructor.
//-----------------------------------------------------------------------------
xcv_twoview_manager::~xcv_twoview_manager() { }

//-----------------------------------------------------------------------------
//: Set the tableau at the given position to be the given tableau.
//-----------------------------------------------------------------------------
void xcv_twoview_manager::set_tableau(vgui_tableau_sptr const& tab, unsigned tab_nb)
{
  if (tab_nb > 1)
  {
    vgui_macro_warning << "Tableau position [" << tab_nb << "] out of range\n";
    return;
  }
#ifdef DEBUG
  std::cerr << "xcv_two_view_manager: Setting tab position [" << tab_nb
           << "] to tableau pointer: " << tab << std::endl;
#endif
  tabs[tab_nb] = tab;
  rubberbands[tab_nb].vertical_cast(vgui_find_below_by_type_name(tab,
    std::string("vgui_rubberband_tableau")));
  if (! rubberbands[tab_nb])
    vgui_macro_warning << "Unable to find rubberbander for tableau1\n";
  easys[tab_nb].vertical_cast(vgui_find_below_by_type_name(tab,
    std::string("vgui_easy2D_tableau")));
  if (!easys[tab_nb]) {
    vgui_macro_warning << "Unable to find easy2D for tableau no. " << tab_nb
                       << " \"" << tab << "\"\n";
    vgui_text_graph(std::cerr);
  }
}

//-----------------------------------------------------------------------------
//: Toggle between displaying and not displaying the FMatrix.
//-----------------------------------------------------------------------------
void xcv_twoview_manager::toggle_f_matrix_display()
{
  if (!f_matrix_is_displayed)
    f_matrix_is_displayed = true;
  else
    f_matrix_is_displayed = false;
}

//-----------------------------------------------------------------------------
//: Toggle between displaying and not displaying the HMatrix2D.
//-----------------------------------------------------------------------------
void xcv_twoview_manager::toggle_h_matrix_display()
{
  if (!h_matrix_is_displayed)
    h_matrix_is_displayed = true;
  else
    h_matrix_is_displayed = false;
}

//-----------------------------------------------------------------------------
//: Toggle between displaying and not displaying corner matches.
//-----------------------------------------------------------------------------
void xcv_twoview_manager::toggle_corner_match_display()
{
  if (!corner_matches_are_displayed)
    corner_matches_are_displayed = true;
  else
    corner_matches_are_displayed = false;
}

//-----------------------------------------------------------------------------
//: Draw a point and the transformed epipolar line on the display.
//-----------------------------------------------------------------------------
void xcv_twoview_manager::draw_f_matrix(vgui_event const& e, vgui_tableau_sptr const& child_tab,
                                        bool make_permanent)
{
  // Get the address of the event and turn it into a HomgPoint2D:
  vgui_projection_inspector p_insp;
  float ix, iy;
  p_insp.window_to_image_coordinates(e.wx, e.wy, ix, iy);
  HomgPoint2D hp(ix, iy);

  HomgLine2D hl;
  if (child_tab == tabs[0])
  {
    hl = f_matrix->image2_epipolar_line(hp);
    transfer_index = 1;
  }
  else if (child_tab == tabs[1])
  {
    hl = f_matrix->image1_epipolar_line(hp);
    transfer_index = 0;
  }
  else
  {
    vgui_macro_warning << "Unknown child tableau: " << child_tab << std::endl;
    return;
  }

  // Draw the line and points on the appropriate tableau:
  if (make_permanent)
  {
#ifdef DEBUG
    std::cerr << "Drawing the infinite line: " << hl[0] << "x + "
             << hl[1] << "y + " << hl[2] << " = 0.\n";
#endif
    if (easys[transfer_index])
      easys[transfer_index]->add_infinite_line(float(hl[0]), float(hl[1]), float(hl[2]));
    else
      vgui_macro_warning << "no vgui_easy2D_tableau for transfer_index = "
                         << transfer_index << std::endl;
    if (easys[(transfer_index+1)%2])
      easys[(transfer_index+1)%2]->add_point(ix, iy);
    else
      vgui_macro_warning << "no vgui_easy2D_tableau for transfer_index = "
                         << (transfer_index+1)%2 << std::endl;
    if (easys[0])
      easys[0]->post_redraw();
    else
      vgui_macro_warning << "no vgui_easy2D_tableau at index 0\n";
  }
  else
  {
    line_coord_a = float(hl[0]);
    line_coord_b = float(hl[1]);
    line_coord_c = float(hl[2]);
    event_coord_x = ix; event_coord_y = iy;
    if (use_overlays)
      tabs[transfer_index]->post_overlay_redraw();
    else
      tabs[transfer_index]->post_redraw();
  }
}

//-----------------------------------------------------------------------------
//: Handle overlay redraw event for FMatrix.
//-----------------------------------------------------------------------------
void xcv_twoview_manager::draw_overlay_f_matrix(vgui_tableau_sptr const& child_tab)
{
  if (child_tab == tabs[transfer_index])
  {
    // Force a re-draw of the image and easy before we draw on top of it:
    vgui_event evt;
    if (!use_overlays)
      evt.type = vgui_DRAW;
    else
      evt.type = vgui_DRAW_OVERLAY;
    easys[transfer_index]->handle(evt);
    rubberbands[transfer_index]->draw_infinite_line(line_coord_a,
                                                    line_coord_b, line_coord_c);
  }
}

//-----------------------------------------------------------------------------
//: Draw the point and corresponding point computed using the HMatrix2D.
//-----------------------------------------------------------------------------
void xcv_twoview_manager::draw_h_matrix(
  vgui_event const& e, vgui_tableau_sptr const& child_tab, bool make_permanent)
{
  // Get the address of the event and turn it into a HomgPoint2D:
  vgui_projection_inspector p_insp;
  float ix, iy;
  p_insp.window_to_image_coordinates(e.wx, e.wy, ix, iy);
  HomgPoint2D hp(ix, iy);

  HomgPoint2D transformed_hp;
  if (child_tab == tabs[0])
  {
    transformed_hp = h_matrix->transform_to_plane2(hp);
    transfer_index = 1;
  }
  else if (child_tab == tabs[1])
  {
    transformed_hp = h_matrix->transform_to_plane1(hp);
    transfer_index = 0;
  }
  else
  {
    vgui_macro_warning << "Unknown child tableau: " << child_tab << std::endl;
    return;
  }

  double px, py;
  transformed_hp.get_nonhomogeneous(px, py);

  // Draw the points on the appropriate tableau:
  if (make_permanent)
  {
#ifdef DEBUG
    std::cerr << "draw_h_matrix: Adding points at (" << px << ", " << py
             << ") and (" << ix << ", " << iy << ").\n";
#endif
    easys[transfer_index]->add_point(float(px), float(py));
    easys[(transfer_index+1)%2]->add_point(ix, iy);
    easys[0]->post_redraw();
  }
  else
  {
    event_coord_x = ix; event_coord_y = iy;
    point_coord_x = float(px); point_coord_y = float(py);
    if (use_overlays)
      rubberbands[transfer_index]->post_overlay_redraw();
    else
    rubberbands[transfer_index]->post_redraw();
  }
}

//-----------------------------------------------------------------------------
//: Handle overlay redraw event for HMatrix2D.
//-----------------------------------------------------------------------------
void xcv_twoview_manager::draw_overlay_h_matrix(vgui_tableau_sptr const& child_tab)
{
  if (child_tab == tabs[transfer_index])
  {
    // Force a re-draw of the image and easy before we draw on top of it:
    vgui_event evt;
    if (!use_overlays)
      evt.type = vgui_DRAW;
    else
      evt.type = vgui_OVERLAY_DRAW;
    easys[transfer_index]->handle(evt);

    // Draw a cross-hair over the point:
    int crosshair_radius = 8;
    rubberbands[transfer_index]->draw_line(point_coord_x-crosshair_radius,
                                           point_coord_y, point_coord_x+crosshair_radius, point_coord_y);
    rubberbands[transfer_index]->draw_line(point_coord_x,
                                           point_coord_y+crosshair_radius, point_coord_x,
                                           point_coord_y-crosshair_radius);
    rubberbands[transfer_index]->draw_circle(point_coord_x, point_coord_y,
                                             crosshair_radius);
  }
}

//-----------------------------------------------------------------------------
//: Draw matching corners in two views.
//-----------------------------------------------------------------------------
void xcv_twoview_manager::draw_corner_matches(
  vgui_event const&, vgui_tableau_sptr const&child_tab)
{
  if (child_tab == tabs[0])
  {
    transfer_index = 1;
  }
  else if (child_tab == tabs[1])
  {
    transfer_index = 0;
  }
  else
  {
    vgui_macro_warning << "Unknown child tableau: " << child_tab << std::endl;
    return;
  }
}

//-----------------------------------------------------------------------------
//: Handle overlay redraw event for corner matches.
//  This is not completed yet.... FIXME
//-----------------------------------------------------------------------------
void xcv_twoview_manager::draw_overlay_corner_matches(vgui_tableau_sptr const&)
{
  std::cerr << "xcv_twoview_manager::draw_overlay_corner_matches(vgui_tableau_sptr) is not yet implemented\n";
#if 0 // commented out - FIXME
  // Get the currently highlighted point:
  vgui_soview* sv = easys[(transfer_index+1)%2]->get_highlighted_soview();
  if (sv->type_name() != "vgui_soview2D_point")
    return;

  vgui_soview2D_point* sv_point = (vgui_soview2D_point*)sv;

  HomgInterestPointSet* hips;
  if (transfer_index == 1)
    hips = corner_matches->get_corners1();
  else
    hips = corner_matches->get_corners2();

  int j = -1;
  double xx, yy;
  do
  {
    j++;
    hips->get(j).homg_.get_nonhomogeneous(xx, yy);
  }
  while (j<corner_matches->size() && sv_point->x != xx & sv_point->y != yy);

  if (j == corner_matches->size()) // Corner is not in matched list.
    return;

  int index2 = corner_matches->get_match_12(j);
  if (index2 == MatchSet::NoMatch)
    return;

  HomgPoint2D new_point;
  if (transfer_index == 0)
    new_point = (corner_matches->get_corners1())->get(index2).homg_;
  else
    new_point = (corner_matches->get_corners2())->get(index2).homg_;
  double new_x, new_y;
  new_point.get_nonhomogeneous(new_x, new_y);

  rubberbands[transfer_index]->draw_circle(new_x, new_y, 5);
#endif // 0
}

//-----------------------------------------------------------------------------
//: Handle all events sent to this manager.
//-----------------------------------------------------------------------------
void xcv_twoview_manager::handle_tjunction_event(vgui_event const& e, vgui_tableau_sptr const& child_tab)
{
  if (e.type == vgui_BUTTON_DOWN && e.modifier == vgui_MODIFIER_NULL && e.button == vgui_LEFT)
    dragging = true;
  if (e.type == vgui_BUTTON_UP || e.type == vgui_LEAVE)
  {
    dragging = false;
    rubberbands[0]->post_redraw();
  }
  if ((e.type == vgui_MOTION || e.type == vgui_BUTTON_DOWN) && dragging)
  {
    if (f_matrix != nullptr && f_matrix_is_displayed)
      draw_f_matrix(e, child_tab, false);
    if (h_matrix != nullptr && h_matrix_is_displayed)
      draw_h_matrix(e, child_tab, false);
    if (corner_matches != nullptr && corner_matches_are_displayed)
      draw_corner_matches(e, child_tab);
  }

  if (e.type == vgui_BUTTON_DOWN &&
      e.modifier == vgui_MODIFIER_NULL &&
      e.button == vgui_MIDDLE)
  {
    if (f_matrix != nullptr && f_matrix_is_displayed)
      draw_f_matrix(e, child_tab, true);
    if (h_matrix != nullptr && h_matrix_is_displayed)
      draw_h_matrix(e, child_tab, true);
  }
  if (dragging)
  {
    if ((use_overlays && e.type == vgui_DRAW_OVERLAY) ||
        (!use_overlays && e.type == vgui_DRAW))
    {
#if 0 // TODO awf fix this.
      if (child_tab == tabs[transfer_index])
        rubberbands[transfer_index]->child->handle(e);
#endif // 0

      if (child_tab == tabs[transfer_index])
      {
         easys[transfer_index]->handle(e);
         easys[transfer_index]->get_child(0)->handle(e);
      }

      if (f_matrix != nullptr && f_matrix_is_displayed)
        draw_overlay_f_matrix(child_tab);
      if (h_matrix != nullptr && h_matrix_is_displayed)
        draw_overlay_h_matrix(child_tab);
      if (corner_matches != nullptr && corner_matches_are_displayed)
        draw_overlay_corner_matches(child_tab);
    }
  }
}
