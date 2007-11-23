#ifndef bwm_observable_mesh_h_
#define bwm_observable_mesh_h_

#include "bwm_observable.h"
#include "bwm_observable_sptr.h"
#include "bwm_observable_mesh_sptr.h"

#include <vgui/vgui_message.h>
#include <vsol/vsol_polygon_3d.h>
#include <vsol/vsol_polygon_3d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>

#include <bmsh3d/bmsh3d_mesh_mc.h>
#include <bmsh3d/bmsh3d_face_mc.h>

typedef enum {RoofSurface, WallSurface, None} BWM_FACE_LABEL;


class bwm_observable_mesh : public bwm_observable
{
public:

  typedef enum {BWM_MESH_IMAGE_PROCESSING, BWM_MESH_FEATURE, BWM_MESH_TERRAIN } BWM_MESH_TYPES;
 
  virtual vcl_string type_name() const {return "bwm_observable_mesh"; }

  //: constructors
  bwm_observable_mesh()  : object_(0){}
  bwm_observable_mesh(bmsh3d_mesh_mc* object)  : object_(object) {}
  bwm_observable_mesh(vsol_polygon_3d_sptr poly);
  bwm_observable_mesh(vsol_polygon_3d_sptr poly, double dist);

  //: destructor
  virtual ~bwm_observable_mesh();

  void set_mesh_type(BWM_MESH_TYPES type) { mesh_type_ = type; }

  BWM_MESH_TYPES mesh_type() { return mesh_type_; }

  void remove();

  vgl_box_3d<double> bounding_box();
  
  unsigned num_faces() { return object_->facemap().size(); }
  unsigned num_edges() { return object_->edgemap().size(); }
  unsigned num_vertices() { return object_->vertexmap().size(); }
  void translate(vgl_vector_3d<double> T);
  bwm_observable_sptr transform(vgl_h_matrix_3d<double> T_);
  void extrude(int face_id);

  void extrude(int face_id, double dist);

  bmsh3d_mesh_mc* get_object() { return object_; }

  void set_object(bmsh3d_mesh_mc* obj);
  void set_object(vsol_polygon_3d_sptr poly, double dist);
  void set_object(vsol_polygon_3d_sptr poly);

  void move(vsol_polygon_3d_sptr poly);
  void move_extr_face(double dist);
  void move_normal_dir(double dist);

  void replace(bmsh3d_mesh_mc* obj);

  void replace(vsol_polygon_3d &polygon);

  void attach_inner_face(unsigned face_id, vsol_polygon_3d_sptr poly);

  vcl_map<int, vsol_polygon_3d_sptr> extract_faces();

  vsol_polygon_3d_sptr extract_face(bmsh3d_face_mc* face, 
                                    vcl_vector<bmsh3d_vertex*> &vertices);

  vsol_polygon_3d_sptr extract_face(unsigned i);

  vcl_map<int, vsol_polygon_3d_sptr> extract_inner_faces(bmsh3d_face_mc* face);

  vcl_map<int, vsol_polygon_3d_sptr> extract_inner_faces(int face_id);

  void divide_face(unsigned face_id, vgl_point_3d<double> l1, vgl_point_3d<double> l2,
    vgl_point_3d<double> p1, vgl_point_3d<double> l3, 
    vgl_point_3d<double> l4, vgl_point_3d<double> p2);

  bool is_poly_in(unsigned id, unsigned& index);

  void label_roof(unsigned face_id){ labels_[face_id] = RoofSurface; }
  
  void label_wall(unsigned face_id){ labels_[face_id] = WallSurface; }

  BWM_FACE_LABEL get_face_label(unsigned face_id);

  vgl_homg_plane_3d<double> get_plane(unsigned face_id);

  int find_closest_face(vgl_point_3d<double> point);

  void create_interior();

  void triangulate();

  SoSeparator* convert_coin3d(bool b_shape_hints, float transp, int colorcode);

  bwm_observable_sptr global_to_local(bgeo_lvcs* lvcs, double& min_z);

  void load_from(vcl_string filename);

  void save(const char* filename, bgeo_lvcs* lvcs);

  void save(const char* filename);

private:

  bmsh3d_mesh_mc* object_;

  bmsh3d_face_mc* current_extr_face;

  // the pair is <face_id, inner_face_id> and it is mapped to the polygon id from the UI
  //vcl_map<unsigned, vcl_pair<unsigned, unsigned> > inner_ids_;

  vcl_map<unsigned, BWM_FACE_LABEL> labels_;

  BWM_MESH_TYPES mesh_type_;

  void create_mesh_HE(vsol_polygon_3d_sptr polygon, double dist, vcl_map<int, vsol_polygon_3d_sptr> inner_faces);

  bmsh3d_face* create_inner_face(vsol_polygon_3d_sptr polygon);

  bmsh3d_face_mc* create_face(vsol_polygon_3d_sptr polygon);

  bmsh3d_face_mc* extrude_face(bmsh3d_mesh_mc* M, bmsh3d_face_mc* F);

  void notify_observers(vcl_string message_type);

  void move_points_to_plane(bmsh3d_face_mc* face);

  void shrink_mesh(bmsh3d_mesh_mc* mesh, double dist);

  void print_faces();
};

#endif
