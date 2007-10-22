#ifndef bwm_observer_img_h_
#define bwm_observer_img_h_

#include <vcl_vector.h>
#include <vcl_map.h>

#include <vgui/vgui_easy2d_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_soview2D.h>

#include <bgui/bgui_image_tableau.h>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_plane_3d.h>
#include <vgl/vgl_homg_point_2d.h>

#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_3d_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_3d_sptr.h>

class bwm_observer_img : public vgui_easy2D_tableau
{
public:

  bwm_observer_img(bgui_image_tableau_sptr const& img)
    : vgui_easy2D_tableau(img), img_tab_(img), viewer_(0){  }

  // set the initial projection plane to z=0
 // bwm_observer_img(bgui_image_tableau_sptr const& img, const char* n="unnamed")
 //   : img_tab_(img), viewer_(0) { }

  virtual ~bwm_observer_img(){}

  bgui_image_tableau_sptr image_tableau() { return img_tab_; }

  void set_viewer(vgui_viewer2D_tableau_sptr viewer) { viewer_ = viewer; }

  //bool handle(const vgui_event &);

  virtual vcl_string type_name() const { return "bwm_observer_img"; }

  void create_polygon(vsol_polygon_2d_sptr);

  void delete_polygon();

  void delete_all();

  void save();

  void hist_plot();

  void intensity_profile(float start_col, float start_row,
                         float end_col, float end_row);

  void range_map();

  //void triangulate_meshes();

  //void move_corr_point(vsol_point_2d_sptr new_pt);
  //void world_pt_corr();

protected:

  bgui_image_tableau_sptr img_tab_;

  vgui_viewer2D_tableau_sptr viewer_;

  // polygons are mapped soview ID
  vcl_map<unsigned, vgui_soview2D_polygon*> poly_list;

  // vector of vertices are mapped soview ID for each polygon
  vcl_map<unsigned, vcl_vector<vgui_soview2D_circle* > > vert_list;

  void delete_polygon(vgui_soview* obj);

};

#endif
