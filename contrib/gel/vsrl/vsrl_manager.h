//
// vdmt_manager.cxx
//
// This program was written to test the Dense Matching software
//
// G.W. Brooksby
// 02/13/03

#ifndef vdmt_manager_h_
#define vdmt_manager_h_

#include <vil/vil_image.h>
#include <vil/vil_memory_image_of.h>
#include <vgui/vgui_image_tableau_sptr.h>
#include <vgui/vgui_wrapper_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vsrl/vsrl_parameters.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_digital_curve_sptr.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_intensity_face_sptr.h>

#include "vsrl_point_picker.h"

class vsrl_manager : public vgui_wrapper_tableau
{
public:
  ~vsrl_manager();
  static vsrl_manager *instance();
  void quit();
  void load_left_image();
  void load_right_image();
  void load_disparity_image();
  void save_disparity_image();
  void load_params_file();
  void init();
  void point_pick();
  void clear_all();
  virtual bool handle(vgui_event const &);
  bool validate_point(vnl_vector<float>* pt);
  int get_disparity(vnl_vector<float>* pt);
  bool put_points();
  bool put_lines();
  bool do_dense_matching();
  void find_regions();
  void draw_regions(vcl_vector<vdgl_intensity_face_sptr>& regions, bool verts);
  void set_params();
  vil_image scale_image(vil_memory_image_of<unsigned char> img);

private:
  vsrl_manager();
  vil_image imgL_;
  vil_image imgR_;
  vil_image disp_img_;
  vsrl_parameters* params_;
  vgui_grid_tableau_sptr grid_;
  vgui_image_tableau_sptr itabL_;
  vgui_image_tableau_sptr itabR_;
  vgui_easy2D_tableau_sptr e2d0_;
  vgui_easy2D_tableau_sptr e2d1_;
  vgui_easy2D_tableau_sptr e2d2_;
  vsrl_point_picker_sptr vpicker0_;
  vsrl_point_picker_sptr vpicker1_;
  vsrl_point_picker_sptr vpicker2_;
  vgui_image_tableau_sptr dimg_tab_;
  static vsrl_manager *instance_;
  int disparity_bias_;
  
};

#endif // vsrl_manager_h_
