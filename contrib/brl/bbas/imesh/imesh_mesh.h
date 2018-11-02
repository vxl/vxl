// This is brl/bbas/imesh/imesh_mesh.h
#ifndef imesh_mesh_h_
#define imesh_mesh_h_
//:
// \file
// \brief An indexed face set mesh
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date May 2, 2008

#include <vector>
#include <iostream>
#include <memory>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

#include <imesh/imesh_vertex.h>
#include <imesh/imesh_face.h>
#include <imesh/imesh_half_edge.h>

#include <vgl/vgl_point_2d.h>

//for brdb smart pointer
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vsl/vsl_binary_io.h>

//: A simple mesh
class imesh_mesh : public vbl_ref_count
{
 public:
  //: Default Constructor
  imesh_mesh() : tex_coord_status_(TEX_COORD_NONE) {}

  //: Constructor from vertex and face arrays
  //  Takes ownership of these arrays
  imesh_mesh(std::unique_ptr<imesh_vertex_array_base> verts, std::unique_ptr<imesh_face_array_base> faces)
  : verts_(std::move(verts)), faces_(std::move(faces)), tex_coord_status_(TEX_COORD_NONE) {}

  //: Copy Constructor
  imesh_mesh(const imesh_mesh& other);

  //: Assignment operator
  imesh_mesh& operator=(imesh_mesh const& other);

  //: Return the number of vertices
  unsigned int num_verts() const {return verts_->size();}

  //: Return the number of faces
  unsigned int num_faces() const {return faces_->size();}

  //: Return the number of edges
  unsigned int num_edges() const {return half_edges_.size()/2;}

  //: Merge the data from another mesh into this one
  //  Duplicates are not removed
  void merge(const imesh_mesh& other);

  //: Return true if the mesh has been initialized
  bool is_init() const { return verts_.get() && faces_.get(); }

  //: Access the vector of vertices
  const imesh_vertex_array_base& vertices() const { return *verts_; }
  imesh_vertex_array_base& vertices() { return *verts_; }

  //: Access the vector of vertices cast to a dimension
  template <unsigned int d>
  const imesh_vertex_array<d>& vertices() const
  {
    assert(dynamic_cast<imesh_vertex_array<d>*>(verts_.get()));
    return static_cast<const imesh_vertex_array<d>&>(*verts_);
  }
  template <unsigned int d>
  imesh_vertex_array<d>& vertices()
  {
    assert(dynamic_cast<imesh_vertex_array<d>*>(verts_.get()));
    return static_cast<imesh_vertex_array<d>&>(*verts_);
  }

  //: Access the vector of faces
  const imesh_face_array_base& faces() const { return *faces_; }
  imesh_face_array_base& faces() { return *faces_; }

  //: Set the vertices
  void set_vertices(std::unique_ptr<imesh_vertex_array_base> verts) { verts_ = std::move(verts); }

  //: Set the faces
  void set_faces(std::unique_ptr<imesh_face_array_base> faces) { faces_ = std::move(faces); }

  //: Returns true if the mesh has computed half edges
  bool has_half_edges() const { return half_edges_.size() > 0; }

  //: Return the half edge set
  const imesh_half_edge_set& half_edges() const { return half_edges_; }

  //: Construct the half edges graph structure
  void build_edge_graph();

  //: Remove the half edge graph structure
  void remove_edge_graph() { half_edges_.clear(); }

  //: Compute vertex normals
  void compute_vertex_normals();

  //: Compute vertex normals using face normals
  void compute_vertex_normals_from_faces();

  //: Compute face normals
  //  If norm == false the vector lengths are twice the area of the face
  void compute_face_normals(bool norm = true);

  //: This type indicates how texture coordinates are indexed
  // ON_VERT is one coordinate per vertex
  // ON_CORNER is one coordinate per half edge (i.e. corner)
  enum tex_coord_type { TEX_COORD_NONE = 0,
                        TEX_COORD_ON_VERT = 1,
                        TEX_COORD_ON_CORNER = 2 };

  //: Returns texture coordinate availability
  tex_coord_type has_tex_coords() const { return tex_coord_status_; }

  //: Return the texture coordinates
  const std::vector<vgl_point_2d<double> >& tex_coords() const { return tex_coords_; }

  //: Set the texture coordinates
  void set_tex_coords(const std::vector<vgl_point_2d<double> >& tc);

  //: set the texture sources
  void set_tex_source(const std::string ts) { tex_source_ = ts; }
  const std::string& tex_source() const { return tex_source_; }

  //: Return a vector indicating which faces have texture
  const std::vector<bool>& valid_tex_faces() const { return valid_tex_faces_; }

  //: Set the vector indicating which faces have texture
  void set_valid_tex_faces(const std::vector<bool>& valid);

  //: Label all faces with positive (counter clockwise orientation) area as valid
  //  This requirement refers to the texture map coordinates
  void label_ccw_tex_faces_valid();

  //: Map a barycentric coordinate (u,v) on triangle \param tri into texture space
  vgl_point_2d<double> texture_map(unsigned int tri,
                                   double u, double v) const;


 private:
  std::unique_ptr<imesh_vertex_array_base> verts_;
  std::unique_ptr<imesh_face_array_base> faces_;
  imesh_half_edge_set half_edges_;

  //: vector of texture coordinates
  std::vector<vgl_point_2d<double> > tex_coords_;

  //: vector of texture sources
  std::string tex_source_;

  //: indicate which faces have texture data
  std::vector<bool> valid_tex_faces_;
  //: the type of texture coordinates
  tex_coord_type tex_coord_status_;
};

//smartptr
typedef vbl_smart_ptr<imesh_mesh> imesh_mesh_sptr;

//--- IO read/write for sptrs--------------------------------------------------
//: Binary write boxm2_scene scene to stream
void vsl_b_write(vsl_b_ostream& os, imesh_mesh_sptr& sptr);
void vsl_b_write(vsl_b_ostream& os, imesh_mesh_sptr const& sptr);

//: Binary load boxm2_scene scene from stream.
void vsl_b_read(vsl_b_istream& is, imesh_mesh_sptr& sptr);
void vsl_b_read(vsl_b_istream& is, imesh_mesh_sptr const& sptr);

#endif // imesh_mesh_h_
