// This is brl/bbas/imesh/imesh_mesh.h
#ifndef imesh_mesh_h_
#define imesh_mesh_h_

//:
// \file
// \brief An indexed face set mesh 
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 5/2/08
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_vector.h>
#include <vcl_memory.h>

#include <imesh/imesh_vertex.h>
#include <imesh/imesh_face.h>
#include <imesh/imesh_half_edge.h>

#include <vgl/vgl_point_3d.h>

//: A simple mesh
class imesh_mesh
{
  public:
    //: Default Constructor
    imesh_mesh() : tex_coord_status_(TEX_COORD_NONE) {}

    //: Constructor from vertex and face arrays
    // takes ownership of these arrays
    imesh_mesh(vcl_auto_ptr<imesh_vertex_array_base> verts, vcl_auto_ptr<imesh_face_array_base> faces)
    : verts_(verts), faces_(faces), tex_coord_status_(TEX_COORD_NONE) {}

    //: Copy Constructor
    imesh_mesh(const imesh_mesh& other);

    //: Return the number of vertices
    unsigned int num_verts() const {return verts_->size();}

    //: Return the number of faces
    unsigned int num_faces() const {return faces_->size();}

    //: Return the number of edges
    unsigned int num_edges() const {return half_edges_.size()/2;}

    //: Merge the data from another mesh into this one
    // duplicates are not removed
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
    void set_vertices(vcl_auto_ptr<imesh_vertex_array_base> verts) { verts_ = verts; }

    //: Set the faces
    void set_faces(vcl_auto_ptr<imesh_face_array_base> faces) { faces_ = faces; }

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
    // if norm == false the vector lengths are twice the area of the face
    void compute_face_normals(bool norm = true);

    //: This type indicates how texture coordinates are indexed
    // ON_VERT is one coordinate per vertex
    // ON_CORNER is one coordinate per half edge (i.e. corner)
    enum tex_coord_type { TEX_COORD_NONE = 0,
                          TEX_COORD_ON_VERT = 1,
                          TEX_COORD_ON_CORNER = 2 };

    //: Returns texture coordinate availablity
    tex_coord_type has_tex_coords() const { return tex_coord_status_; }

    //: Return the texture coordinates
    const vcl_vector<vgl_point_2d<double> >& tex_coords() const { return tex_coords_; }

    //: Set the texture coordinates
    void set_tex_coords(const vcl_vector<vgl_point_2d<double> >& tc);

    //: Map a barycentric coordinate (u,v) on triangle \param tri into texture space
    vgl_point_2d<double> texture_map(unsigned int tri,
                                     double u, double v) const;


  private:
    vcl_auto_ptr<imesh_vertex_array_base> verts_;
    vcl_auto_ptr<imesh_face_array_base> faces_;
    imesh_half_edge_set half_edges_;

    vcl_vector<vgl_point_2d<double> > tex_coords_;
    tex_coord_type tex_coord_status_;
};




#endif // imesh_mesh_h_
