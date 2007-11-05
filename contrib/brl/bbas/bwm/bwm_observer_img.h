#ifndef bwm_observer_img_h_
#define bwm_observer_img_h_

#include <vcl_vector.h>
#include <vcl_map.h>

#include <bwm/algo/bwm_soview2D_vertex.h>

#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_soview2D.h>

#include <bgui/bgui_image_tableau.h>
#include <bgui/bgui_vsol2D_tableau.h>
#include <bgui/bgui_vsol_soview2D.h>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_plane_3d.h>
#include <vgl/vgl_homg_point_2d.h>

#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_3d_sptr.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_3d_sptr.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_polyline_3d_sptr.h>

class bwm_observer_img : public bgui_vsol2D_tableau
{
public:

  bwm_observer_img(bgui_image_tableau_sptr const& img)
    : bgui_vsol2D_tableau(img), img_tab_(img), viewer_(0){  }

  virtual ~bwm_observer_img(){}

  bgui_image_tableau_sptr image_tableau() { return img_tab_; }

  void set_viewer(vgui_viewer2D_tableau_sptr viewer) { viewer_ = viewer; }

  bool handle(const vgui_event &);

  virtual vcl_string type_name() const { return "bwm_observer_img"; }

  void create_box(vsol_box_2d_sptr);

  void create_polygon(vsol_polygon_2d_sptr);

  void create_polyline(vsol_polyline_2d_sptr);

  void create_point(vsol_point_2d_sptr);

  bool get_selected_box(vsol_box_2d_sptr & box);

  void delete_selected();

  void delete_all();

  void save();

  void hist_plot();

  void intensity_profile(float start_col, float start_row,
                         float end_col, float end_row);

  void range_map();

  void step_edges_vd();

  void lines_vd();

  void jim_obs_process(){ vcl_cout << "I am Jim Green's Process!!!!" << vcl_endl; }

  void toggle_show_image_path();

protected:

  bwm_observer_img();

  bgui_image_tableau_sptr img_tab_;

  vgui_viewer2D_tableau_sptr viewer_;

  bool show_image_path_;

  // polygons are mapped soview ID
  vcl_map<unsigned, vgui_soview2D*> obj_list;

  // vector of vertices are mapped soview ID for each polygon
  vcl_map<unsigned, vcl_vector<bwm_soview2D_vertex* > > vert_list;

  float start_x_, start_y_;
  bgui_vsol_soview2D_polyline* moving_p_;
  bwm_soview2D_vertex* moving_v_;
  bool moving_vertex_, moving_polygon_;

  void delete_polygon(vgui_soview* obj);

  bgui_vsol_soview2D* get_selected_object(vcl_string type);
};

#endif
