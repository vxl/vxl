//this-sets-emacs-to-*-c++-*-mode
#ifndef segv_segmentation_manager_h_
#define segv_segmentation_manager_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief Manager for segmentation algorithm execution and display

// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy November 18, 2002    Initial version.
// \endverbatim
//--------------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vil/vil_image.h>
#include <vil/vil_memory_image_of.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_image_tableau_sptr.h>
#include <vgui/vgui_easy2D_tableau_sptr.h>
#include <vgui/vgui_viewer2D_tableau_sptr.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_window.h>
#include <segv/segv_vtol2D_tableau_sptr.h>
//: A manager for displaying segmentation results.  

class segv_segmentation_manager : public vgui_grid_tableau
{
 public:
  segv_segmentation_manager();
  ~segv_segmentation_manager();
  static segv_segmentation_manager *instance();
  void quit();
  void load_image();
  void clear_display();
  void vd_edges();
  void clean_vd_edges();
  //: access to the window
  vgui_window* get_window(){return _win;}
  void set_window(vgui_window* win){_win=win;}

  //: the virtual handle function
  virtual bool handle(const vgui_event&);
 protected:
  void draw_edges(vcl_vector<vtol_edge_2d_sptr>& edges, bool verts=false);


 private:
  //utility functions

  //flags

  vil_image _img;
  vgui_window* _win;
  //  vgui_easy2D_tableau_sptr _e2D;
  segv_vtol2D_tableau_sptr _t2D;
  static segv_segmentation_manager *_instance;
};

#endif // segv_segmentation_manager_h_
