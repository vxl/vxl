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
//---------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vil/vil_image.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vdgl/vdgl_intensity_face_sptr.h>
#include <vgui/vgui_wrapper_tableau.h>
#include <bgui/bgui_vtol2D_tableau_sptr.h>

class vgui_window;

class segv_segmentation_manager : public vgui_wrapper_tableau
{
 public:
  segv_segmentation_manager();
  ~segv_segmentation_manager();
  static segv_segmentation_manager *instance();
  void quit();
  void load_image();
  void clear_display();
  void original_image();
  void gaussian();
  void convolution();
  void vd_edges();
  void regions();
  void test_face();
  void test_digital_lines();
#ifdef HAS_XERCES
  void read_xml_edges();
#endif
  void init();

  //: access to the window
  vgui_window* get_window(){return win_;}
  void set_window(vgui_window* win){win_=win;}

 protected:
  void draw_edges(vcl_vector<vtol_edge_2d_sptr>& edges, bool verts=false);
  void draw_regions(vcl_vector<vdgl_intensity_face_sptr>& regions,
                    bool verts=false);
 private:
  //flags

  vil_image img_;
  vgui_window* win_;
  bgui_vtol2D_tableau_sptr t2D_;
  static segv_segmentation_manager *instance_;
};

#endif // segv_segmentation_manager_h_
