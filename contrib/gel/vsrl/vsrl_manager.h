// vsrl_manager.cxx
//
// This program was written to test the Dense Matching software
//
// G.W. Brooksby
// 02/13/03

#ifndef vsrl_manager_h_
#define vsrl_manager_h_

#include <vil1/vil1_image.h>
#include <vil1/vil1_memory_image_of.h>
#include <vgui/vgui_image_tableau_sptr.h>
#include <vgui/vgui_wrapper_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_soview2D.h>
#include <vsrl/vsrl_parameters.h>
#include <vtol/vtol_intensity_face_sptr.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_point_2d.h>

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
  bool validate_point(vgl_point_2d<float> const& pt);
  int get_disparity(vgl_point_2d<float> const& pt);
  bool put_points();
  bool put_lines();
  bool do_dense_matching();
  void find_regions();
  void draw_regions(vcl_vector<vtol_intensity_face_sptr>& regions, bool verts);
  void set_params();
  void draw_north_arrow();
  void test_func();
  vgui_soview2D_lineseg* draw_vector_at(vgl_vector_2d<float>* vec, float x, float y, float theta);
  vil1_image scale_image(vil1_memory_image_of<unsigned char> img);
  vil1_image scale_image(vil1_memory_image_of<double> img);
  vil1_image show_gradient_mag(vil1_image* im_in);
  vil1_image show_gradient_dir(vil1_memory_image_of<double> im_in);
  vil1_memory_image_of<double> make_3d();

 private:
  vsrl_manager(); // default constructor is private: only one instance allowed
  vil1_image imgL_;
  vil1_image imgR_;
  vil1_image disp_img_;
  vsrl_parameters* params_;
  vgui_grid_tableau_sptr grid_;
  vgui_image_tableau_sptr itabL_;
  vgui_image_tableau_sptr itabR_;
  vgui_image_tableau_sptr dimg_tab_;
  vgui_easy2D_tableau_sptr e2d0_;
  vgui_easy2D_tableau_sptr e2d1_;
  vgui_easy2D_tableau_sptr e2d2_;
  vsrl_point_picker_sptr vpicker0_;
  vsrl_point_picker_sptr vpicker1_;
  vsrl_point_picker_sptr vpicker2_;
  vgl_vector_2d<float> north_;  // North heading expressed as a vector
  vgl_vector_2d<float> sun_az_; // sun angle expressed as a vector
  vgl_vector_2d<float> sun_el_; // sun elevation expressed as a vector
  static vsrl_manager *instance_;
  int disparity_bias_;
};

#endif // vsrl_manager_h_
