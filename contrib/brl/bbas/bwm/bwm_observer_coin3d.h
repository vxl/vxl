#ifndef bwm_observer_coin3d_h_
#define bwm_observer_coin3d_h_
//:
// \file

#include "bwm_observer.h"
#include "bwm_observable.h"

#include <vcl_map.h>
#include <vcl_vector.h>

#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>

#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_plane_3d.h>

#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_perspective_camera.h>

#include <bgui3d/bgui3d_examiner_tableau_sptr.h>
#include <bgui3d/bgui3d_examiner_tableau.h>
#include <bgui3d/bgui3d_project2d_tableau.h>

#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoEventCallback.h>

class bwm_observer_coin3d : public bwm_observer, public bgui3d_examiner_tableau
{
 public:
  //: constructors
  bwm_observer_coin3d(SoNode* root)
    : num_objs_(0), T_(0), obs_picked_(0), bgui3d_examiner_tableau(root) {}

  bwm_observer_coin3d(vpgl_proj_camera<double> *camera, SoNode* root);

  virtual ~bwm_observer_coin3d();

  virtual vcl_string type_name() const { return "bwm_observer_coin3d"; }

  bool handle(const vgui_event& e);

  //void update(vgui_message const& msg);

  void handle_update(vgui_message const& msg, bwm_observable_sptr observable);

  void set_proj_plane(vgl_homg_plane_3d<double> proj_plane) { proj_plane_ = proj_plane; }

  vgl_homg_plane_3d<double> get_proj_plane() { return proj_plane_; }

  void set_ground_plane(double x1, double y1, double x2, double y2);

  void extrude();

  void divide();

  void connect_inner_face(vsol_polygon_2d_sptr poly){};

  void create_inner_face(){}

  void create_interior(){};

  void translate(vgl_vector_3d<double> T){}

  void set_trans(vgl_h_matrix_3d<double> m) { T_ = new vgl_h_matrix_3d<double>(m); }

  void update_all(){};

  void delete_object(){};

  void delete_all(){};

  //static void set_selected(SoNode *node) {selected_nodes.push_back(node);}

  static void set_picked(const SoPickedPoint *pp) {picked_points.push_back(pp);}

  SoNode* root() { return root_; }

 private:
  vpgl_camera<double> *camera_;
  vgl_homg_plane_3d<double> proj_plane_;
  SoSeparator* root_;
  SoSelection* root_sel_;
  vcl_map<bwm_observable_sptr, SoSeparator* > objects;
  //vcl_map<bwm_observable_sptr, dbmsh3d_mesh_mc* > meshes;
  //static vcl_vector<SoNode*> selected_nodes;
  static vcl_vector<const SoPickedPoint*> picked_points;
  bwm_observable_sptr obs_picked_;

  SoNode* node_selected_;
  const SoPickedPoint* point_picked_;
  vgl_point_3d<double> point_3d_;
  double start_x_, start_y_;
  bool left_button_down_, middle_button_down_;
  bool divide_mode_;
  vcl_vector<vgl_point_3d<double> > div_pts_;
  int div_idx_;
  int face_id;
  int num_objs_;
  vgl_h_matrix_3d<double> *T_;

  bwm_observable_sptr find_selected_obs(int &);

  bool find_intersection_points(int face_id, vgl_point_3d<double> i1, vgl_point_3d<double> i2,
    vgl_point_3d<double> &p1, vgl_point_3d<double>& l1, vgl_point_3d<double>& l2,
    vgl_point_3d<double> &p2, vgl_point_3d<double>& l3, vgl_point_3d<double>& l4);

  vcl_string create_obj_name();
};

#endif
