//this-sets-emacs-to-*-c++-*-mode
#ifndef bmvv_multiview_manager_h_
#define bmvv_multiview_manager_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief Manager for multiview gui applications
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy December 11, 2002    Initial version.
// \endverbatim
//--------------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vil/vil_image.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vdgl/vdgl_intensity_face_sptr.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <vgui/vgui_wrapper_tableau.h>
#include <vgui/vgui_easy2D_tableau_sptr.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_grid_tableau_sptr.h>
#include <mvl/FMatrix.h>
#include <bgui/bgui_vtol2D_tableau_sptr.h>
#include <bgui/bgui_picker_tableau_sptr.h>

class vgui_window;

//: A manager for displaying segmentation results.
class bmvv_multiview_manager : public vgui_wrapper_tableau
{
 public:
  bmvv_multiview_manager();
  ~bmvv_multiview_manager();
  static bmvv_multiview_manager *instance();

  //:methods for menu callbacks
  void quit();
  void load_image();
  void clear_display();
  void clear_selected();
  void vd_edges();
  void regions();
  void track_edges();
#ifdef HAS_XERCES
  void read_xml_edges();
#endif
  void show_epipolar_line();
  void select_curve_corres();
  //: access to the window
  vgui_window* get_window(){return win_;}
  void set_window(vgui_window* win){win_=win;}
  void init();
  //: the virtual handle function
  virtual bool handle(const vgui_event&);

  void load_image_file(vcl_string image_filename, bool greyscale, unsigned col, unsigned row);
  void draw_colored_edgel_chain(unsigned col, unsigned row, vdgl_edgel_chain_sptr ec, int label);
  void set_changing_colors(int num, float *r, float *g, float *b);

 protected:
  //:internal utility methods
  void draw_regions(vcl_vector<vdgl_intensity_face_sptr>& regions,
                    bool verts=false);

  FMatrix test_fmatrix();
  bgui_vtol2D_tableau_sptr get_vtol2D_tableau_at(unsigned col, unsigned row);
  bgui_vtol2D_tableau_sptr get_selected_vtol2D_tableau();
  bgui_picker_tableau_sptr get_picker_tableau_at(unsigned col, unsigned row);
  bgui_picker_tableau_sptr get_selected_picker_tableau();
  vil_image get_image_at(unsigned col, unsigned row);

  //
 private:
  vil_image img_;
  vgui_window* win_;
  vcl_vector<bgui_vtol2D_tableau_sptr> vtol_tabs_;
  vcl_vector<vtol_edge_2d_sptr> selected_curves_;
  vgui_grid_tableau_sptr grid_;
  static bmvv_multiview_manager *instance_;
};

#endif // bmvv_multiview_manager_h_
