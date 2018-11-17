#ifndef vsol_mesh_3d_h_
#define vsol_mesh_3d_h_
//:
// \file
// \brief a 3-d mesh based on winged edge topology
// \author J.L. Mundy
// \date May 1, 2016
//
//  this code is ported from bwm_observable_mesh
//  avoid dependencies between gel, brl and on bwm observable links
//
#include <string>
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_plane_3d.h>
#include <vsol/vsol_polygon_3d_sptr.h>
#include <vsol/vsol_line_3d_sptr.h>
#include <vsol/vsol_point_3d_sptr.h>
#include <bmsh3d/bmsh3d_mesh_mc.h>
#include <bmsh3d/bmsh3d_face_mc.h>
#include <vsol/vsol_spatial_object_3d.h>
#include <vsol/vsol_volume_3d.h>
#include "vsol_mesh_3d_sptr.h"
// a proper subclass of vsol but not in vsol library to avoid dependencies
class vsol_mesh_3d : public vsol_volume_3d
{
 public:
 vsol_mesh_3d():mesh_(nullptr),current_extr_face_(nullptr){}
  vsol_mesh_3d(const vsol_box_3d_sptr& box);
  ~vsol_mesh_3d() override{delete mesh_;}

  //: required virtual functions for vsol
  vsol_volume_3d_type volume_type(void) const override { return vsol_volume_3d::MESH; }
  bool in(vsol_point_3d_sptr const& p) const override;
  double volume(void) const override{return 0.0;}
  void compute_bounding_box() const override;
  vsol_spatial_object_3d* clone() const override{return nullptr;}

  //: old style downcasting methods from vsol
  vsol_mesh_3d* cast_to_mesh() override { return this;}
  vsol_mesh_3d const* cast_to_mesh() const override { return this;}

  unsigned num_faces() const  { return mesh_->facemap().size(); }
  unsigned num_edges() const { return mesh_->edgemap().size(); }
  unsigned num_vertices() const { return mesh_->vertexmap().size(); }
  std::vector<vsol_point_3d_sptr> vertices() const;


  bmsh3d_mesh_mc* get_object() { return mesh_; }

  void set_mesh(bmsh3d_mesh_mc* obj);
  void set_mesh(const vsol_polygon_3d_sptr& poly, double dist);
  void set_mesh(const vsol_polygon_3d_sptr& poly);

  void attach_inner_face(unsigned face_id, const vsol_polygon_3d_sptr& poly);

  std::map<int, vsol_polygon_3d_sptr> extract_faces();

  vsol_polygon_3d_sptr extract_face(bmsh3d_face_mc* face,
                                    std::vector<bmsh3d_vertex*> &vertices);

  vsol_polygon_3d_sptr extract_face(unsigned i);

  std::map<int, vsol_line_3d_sptr> extract_edges();

  std::vector<vsol_point_3d_sptr> extract_vertices();

  std::map<int, vsol_polygon_3d_sptr> extract_inner_faces(bmsh3d_face_mc* face);

  std::map<int, vsol_polygon_3d_sptr> extract_inner_faces(int face_id);

  vsol_polygon_3d_sptr extract_bottom_face(void);
  vsol_polygon_3d_sptr extract_top_face(void);

  void divide_face(unsigned face_id, vgl_point_3d<double> l1, vgl_point_3d<double> l2,
                   vgl_point_3d<double> p1, vgl_point_3d<double> l3,
                   vgl_point_3d<double> l4, vgl_point_3d<double> p2);

  bool is_poly_in(unsigned id, unsigned& index);

  vgl_plane_3d<double> get_plane(unsigned face_id);

  int find_closest_face(vgl_point_3d<double> point);

  void create_interior();

  void extrude(int face_id, double dist);

  //: requried for vsol serialization
  //: Return a platform independent string identifying the class
  std::string is_a() const override { return std::string("vsol_mesh_3d"); }
  //: Return true if the argument matches the string identifying the class or any parent class
  bool is_class(const std::string& cls) const override { return cls==is_a(); }

 protected:
  bmsh3d_mesh_mc* mesh_;
  bmsh3d_face_mc* current_extr_face_;


  void create_mesh_HE(const vsol_polygon_3d_sptr& polygon, double dist, std::map<int, vsol_polygon_3d_sptr> inner_faces);

  bmsh3d_face* create_inner_face(vsol_polygon_3d_sptr polygon);

  bmsh3d_face_mc* create_face(const vsol_polygon_3d_sptr& polygon);

  bool single_face_with_vertices(unsigned face_id, vsol_polygon_3d_sptr& poly,
                                 std::vector<bmsh3d_vertex*>& verts);
  void move_extr_face(double z);

  bmsh3d_face_mc* extrude_face(bmsh3d_mesh_mc* M, bmsh3d_face_mc* F);

  void move_points_to_plane(bmsh3d_face_mc* face);

  void shrink_mesh(bmsh3d_mesh_mc* mesh, double dist);

  void print_faces();

};
#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vsol_mesh_3d.h
