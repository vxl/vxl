#ifndef bwm_tableau_cam_h_
#define bwm_tableau_cam_h_
//:
// \file

#include <iostream>
#include <string>
#include "bwm_observable.h"
#include "bwm_observer_cam.h"
#include "bwm_tableau_img.h"

#include <vgui/vgui_menu.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_command.h>

#include <bgui/bgui_picker_tableau.h>

#include <vsol/vsol_point_3d_sptr.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


class bwm_tableau_cam : public bwm_tableau_img
{
 public:
  typedef enum {RATIONAL=0, PROJECTIVE=1} BWM_CAMERA_TYPES;

  //: destructor
  // Tableaux are responsible for deleting their observers
  virtual ~bwm_tableau_cam() { delete my_observer_; }

  virtual std::string type_name() const { return "bwm_tableau_cam"; }

  bool handle(const vgui_event& e);

  void set_observer(bwm_observer_cam* obs) { my_observer_ = obs;}

  bwm_observer_cam* observer() const { return this->my_observer_; }

  void get_popup(vgui_popup_params const &params, vgui_menu &menu);

  virtual void create_polygon_mesh();

  virtual void triangulate_mesh();

  // sets this tableau as the master for camera direction
  void set_master();

  //: set the observer as per the image type
  void set_eo();
  void set_other_mode();

  //: moves the object, by moving the selected vertex to the specified point on the tableau
  void move_obj_by_vertex();

  //: extrudes the selected face,
  void extrude_face();

  //: draws a line onto a given face, dividing into two faces
  void divide_face();

  //: Draws the histogram on a graph (if the image is greyscale)
  void hist_plot();

  void set_corr(double X, double Y);

  void move_corr();

  void set_corr_to_vertex();

  void world_pt_corr();

  void load_mesh();

  void load_mesh_multiple();

  void scan_regions();

  void show_geo_position();

  void geo_position_vertex();

  void local_position_vertex();

  void scroll_to_point(double lx, double ly, double lz);

  void create_terrain();

  void create_circular_polygon();

  void project_shadow();
  //------------ save methods

  //: saves the observables with their save method specific to the object
  void save();
  void save_all();
  void save(std::string path);
  void save_all(std::string path);

  //: deletes the selected object
  void delete_object();

  //: deletes all 3D objects
  void delete_all();

  //------------ Projection Plane related methods

  //: selects an existing polygon as the projection plane
  void select_proj_plane();

  //: defines a vertical projection plane passing through a specified line
  void define_proj_plane();

  //: specify a x-y plane as the projection plane with a given z
  void define_xy_proj_plane();

  //: specify a y-z plane as the projection plane with a given x
  void define_yz_proj_plane();

  //: specify a x-z plane as the projection plane with a given x
  void define_xz_proj_plane();

  //: add inner face to an existing face
  void create_inner_face();

  void create_interior();
  void label_roof();
  void label_wall();
  void help_pop();

  void set_draw_mode(int mode) { my_observer_->set_draw_mode((bwm_observer::BWM_DRAW_MODE)mode); }
  void set_mesh_mode() { my_observer_->set_mesh_mode(); }
  void set_face_mode() { my_observer_->set_face_mode(); }
  void set_edge_mode() { my_observer_->set_edge_mode(); }
  void set_vertex_mode() { my_observer_->set_vertex_mode(); }
#if 0
  void create_boxm_scene() { my_observer_->create_boxm_scene(); }
  void load_boxm_scene() { my_observer_->load_boxm_scene(); }
#endif
  bool show_vertices_;
  //==================  camera calibration methods ===============
  void set_horizon() {my_observer_->set_horizon();}
  void set_focal_length();
  void set_cam_height();
  void calibrate_cam_from_horizon()
    {my_observer_->calibrate_cam_from_horizon();}
  void toggle_cam_horizon(){my_observer_->toggle_cam_horizon();}
  std::vector<std::string> set_land_types();
  std::vector<std::string> set_orient_types();
#if 0
  void set_ground_plane(){ my_observer_->set_ground_plane();}
  void set_sky(){ my_observer_->set_sky();}
  void add_vertical_depth_region();
#endif
  void add_ground_plane();
  void add_sky();
  void add_region();
  void edit_region_props();
  void edit_region_weights();
  void save_depth_map_scene();
 protected:
  bwm_observer_cam* my_observer_;
  bwm_tableau_cam(bwm_observer_cam* obs)
    : bwm_tableau_img(obs), my_observer_(obs) {}
};

#endif
