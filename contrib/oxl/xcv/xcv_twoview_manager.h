#ifndef xcv_twoview_manager_h_
#define xcv_twoview_manager_h_

//--------------------------------------------------------------------------------
// .NAME	xcv_twoview_manager
// .INCLUDE	xcv/xcv_twoview_manager.h
// .FILE	xcv_twoview_manager.cxx
// .SECTION Description:
//
// .SECTION Author
//   K.Y.McGaul
// .SECTION Modifications:
//   K.Y.McGaul     05-MAY-2000    Initial version.
//   Marko Bacic    18-AUG-2000    Sorted out display of epipolar lines
//--------------------------------------------------------------------------------

#include <mvl/FMatrix.h>
#include <mvl/HMatrix2D.h>
#include <mvl/PairMatchSetCorner.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_rubberbander.h>
#include <vgui/vgui_easy2D.h>
#include "xcv_mview_manager.h"

class vgui_event;

class xcv_twoview_manager : public xcv_mview_manager
{
public:

  xcv_twoview_manager();
  ~xcv_twoview_manager();

  void set_tableau(vgui_tableau_ref const& tab, unsigned tab_position) ;
  void set_f_matrix(FMatrix* fm){f_matrix = fm;}
  void set_h_matrix(HMatrix2D* hm){h_matrix = hm;}
  void set_corner_matches(PairMatchSetCorner* pmsc){corner_matches = pmsc;}

  void toggle_f_matrix_display();
  void toggle_h_matrix_display();
  void toggle_corner_match_display();

  FMatrix* get_f_matrix(){return f_matrix;}
  HMatrix2D* get_h_matrix(){return h_matrix;}
  PairMatchSetCorner* get_corner_matches(){return corner_matches;}

  void handle_tjunction_event(vgui_event const& e, vgui_tableau_ref const& child_tab);

private:
  FMatrix* f_matrix;
  HMatrix2D* h_matrix;
  PairMatchSetCorner* corner_matches;
  vgui_tableau_ref tabs[2];
  vgui_rubberbander_ref rubberbands[2];
  vgui_easy2D_ref easys[2];
  bool f_matrix_is_displayed;
  bool h_matrix_is_displayed;
  bool corner_matches_are_displayed;
  unsigned transfer_index;
  bool dragging;
  float event_coord_x, event_coord_y;
  float line_coord_a, line_coord_b, line_coord_c;
  float point_coord_x, point_coord_y;

  void draw_f_matrix(vgui_event const& e, vgui_tableau_ref const& child_tab, bool make_permanent);
  void draw_overlay_f_matrix(vgui_tableau_ref const& child_tab);
  void draw_h_matrix(vgui_event const& e, vgui_tableau_ref const& child_tab, bool make_permanent);
  void draw_overlay_h_matrix(vgui_tableau_ref const& child_tab);
  void draw_corner_matches(vgui_event const& e, vgui_tableau_ref const& child_tab);
  void draw_overlay_corner_matches(vgui_tableau_ref const& child_tab);
};

#endif // xcv_twoview_manager_
