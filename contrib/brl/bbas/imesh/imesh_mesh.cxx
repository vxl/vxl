// This is brl/bbas/imesh/imesh_mesh.cxx
#include <iostream>
#include "imesh_mesh.h"
//:
// \file

#include <vgl/vgl_polygon.h>
#include <vgl/vgl_area.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Copy Constructor
imesh_mesh::imesh_mesh(const imesh_mesh& other)
  : vbl_ref_count(),
    verts_((other.verts_.get()) ? other.verts_->clone() : nullptr),
    faces_((other.faces_.get()) ? other.faces_->clone() : nullptr),
    half_edges_(other.half_edges_),
    tex_coords_(other.tex_coords_),
    tex_source_(other.tex_source_),
    valid_tex_faces_(other.valid_tex_faces_),
    tex_coord_status_(other.tex_coord_status_)
{
}


//: Assignment operator
imesh_mesh& imesh_mesh::operator=(imesh_mesh const& other)
{
  if (this != &other) {
    verts_ = std::unique_ptr<imesh_vertex_array_base>((other.verts_.get()) ?
                                                   other.verts_->clone() : nullptr);
    faces_ = std::unique_ptr<imesh_face_array_base>((other.faces_.get()) ?
                                                 other.faces_->clone() : nullptr);
    half_edges_ = other.half_edges_;
    tex_coords_ = other.tex_coords_;
    valid_tex_faces_ = other.valid_tex_faces_;
    tex_coord_status_ = other.tex_coord_status_;
  }
  return *this;
}


//: Merge the data from another mesh into this one
//  Duplicates are not removed
void imesh_mesh::merge(const imesh_mesh& other)
{
  const unsigned num_v = this->num_verts();
  const unsigned num_e = this->num_edges();
  faces_ = imesh_merge(*this->faces_,*other.faces_,verts_->size());
  verts_->append(*other.verts_);

  if (this->has_tex_coords() == TEX_COORD_NONE)
  {
    std::vector<vgl_point_2d<double> > tex;
    if (other.has_tex_coords() == TEX_COORD_ON_VERT) {
      tex = std::vector<vgl_point_2d<double> >(num_v, vgl_point_2d<double>(0,0));
    }
    else if (other.has_tex_coords() == TEX_COORD_ON_CORNER) {
      tex = std::vector<vgl_point_2d<double> >(2*num_e, vgl_point_2d<double>(0,0));
    }

    tex.insert(tex.end(), other.tex_coords().begin(), other.tex_coords().end());
    this->set_tex_coords(tex);
  }
  else if (this->has_tex_coords() == other.has_tex_coords())
  {
    this->tex_coords_.insert(this->tex_coords_.end(),
                             other.tex_coords().begin(),
                             other.tex_coords().end());
  }

  if (this->has_half_edges() && other.has_half_edges())
    this->build_edge_graph();
  else
    this->half_edges_.clear();
}


//: Set the texture coordinates
void imesh_mesh::set_tex_coords(const std::vector<vgl_point_2d<double> >& tc)
{
  if (tc.size() == this->num_verts())
    tex_coord_status_ = TEX_COORD_ON_VERT;
  else if (tc.size() == 2*this->num_edges())
    tex_coord_status_ = TEX_COORD_ON_CORNER;
  else
    tex_coord_status_ = TEX_COORD_NONE;

  tex_coords_ = tc;
}


//: Construct the half edges graph structure
void imesh_mesh::build_edge_graph()
{
  const imesh_face_array_base& faces = this->faces();
  std::vector<std::vector<unsigned int> > face_list(faces.size());
  for (unsigned int f=0; f<faces.size(); ++f) {
    face_list[f].resize(faces.num_verts(f));
    for (unsigned int v=0; v<faces.num_verts(f); ++v)
      face_list[f][v] =  faces(f,v);
  }

  half_edges_.build_from_ifs(face_list);
}


//: Compute vertex normals
void imesh_mesh::compute_vertex_normals()
{
  if (!this->has_half_edges())
    this->build_edge_graph();

  const imesh_half_edge_set& half_edges = this->half_edges();
  imesh_vertex_array<3>& verts = this->vertices<3>();

  std::vector<vgl_vector_3d<double> > normals(this->num_verts(),
                                             vgl_vector_3d<double>(0,0,0));
  std::vector<unsigned int> f_count(this->num_verts(),0);

  for (unsigned int he=0; he < half_edges.size(); ++he) {
    imesh_half_edge_set::f_const_iterator fi(he,half_edges);
    if (fi->is_boundary())
      continue;
    unsigned int vp = fi->vert_index();
    unsigned int v = (++fi)->vert_index();
    unsigned int vn = (++fi)->vert_index();
    normals[v] += normalized(imesh_tri_normal(verts[v],verts[vn],verts[vp]));
    ++f_count[v];
  }

  for (unsigned v=0; v<verts.size(); ++v)
  {
    normals[v] /= f_count[v];
    normalize(normals[v]);
    if (normals[v].length() < 0.5)
      std::cout << "normal "<<v<<" is "<<normals[v] <<std::endl;
  }

  verts.set_normals(normals);
}


//: Compute vertex normals using face normals
void imesh_mesh::compute_vertex_normals_from_faces()
{
  if (!this->has_half_edges())
    this->build_edge_graph();

  if (!this->faces_->has_normals())
    this->compute_face_normals();

  const std::vector<vgl_vector_3d<double> >& fnormals = faces_->normals();

  const imesh_half_edge_set& half_edges = this->half_edges();
  imesh_vertex_array<3>& verts = this->vertices<3>();

  std::vector<vgl_vector_3d<double> > normals(this->num_verts(),
                                             vgl_vector_3d<double>(0,0,0));
  std::vector<unsigned int> f_count(this->num_verts(),0);

  for (unsigned int he=0; he < half_edges.size(); ++he) {
    const imesh_half_edge& half_edge = half_edges[he];
    if (half_edge.is_boundary())
      continue;
    unsigned int v = half_edge.vert_index();
    normals[v] += normalized(fnormals[half_edge.face_index()]);
    ++f_count[v];
  }

  for (unsigned v=0; v<verts.size(); ++v)
  {
    normals[v] /= f_count[v];
    normalize(normals[v]);
    if (normals[v].length() < 0.5)
      std::cout << "normal "<<v<<" is "<<normals[v] <<std::endl;
  }

  verts.set_normals(normals);
}


//: Compute face normals
void imesh_mesh::compute_face_normals(bool norm)
{
  imesh_face_array_base& faces = this->faces();
  const imesh_vertex_array<3>& verts = this->vertices<3>();

  std::vector<vgl_vector_3d<double> > normals(this->num_faces(),
                                             vgl_vector_3d<double>(0,0,0));

  for (unsigned int i=0; i<faces.size(); ++i) {
    const unsigned int num_v = faces.num_verts(i);
    vgl_vector_3d<double>& n = normals[i];
    for (unsigned int j=2; j<num_v; ++j) {
      n += imesh_tri_normal(verts[faces(i,0)],
                            verts[faces(i,j-1)],
                            verts[faces(i,j)]);
    }
    if (norm)
      normalize(n);
  }

  faces.set_normals(normals);
}


//: Map a barycentric coordinate (u,v) on triangle \param tri into texture space
vgl_point_2d<double> imesh_mesh::texture_map(unsigned int tri,
                                             double u, double v) const
{
  vgl_point_2d<double> tex(0,0);
  if (this->tex_coord_status_ == TEX_COORD_ON_VERT)
  {
    unsigned int v1 = (*faces_)(tri,0);
    unsigned int v2 = (*faces_)(tri,1);
    unsigned int v3 = (*faces_)(tri,2);
    tex += (1-u-v)*vgl_vector_2d<double>(tex_coords_[v1].x(),tex_coords_[v1].y());
    tex += u*vgl_vector_2d<double>(tex_coords_[v2].x(),tex_coords_[v2].y());
    tex += v*vgl_vector_2d<double>(tex_coords_[v3].x(),tex_coords_[v3].y());
  }
  return tex;
}


//: Set the vector indicating which faces have texture
void imesh_mesh::set_valid_tex_faces(const std::vector<bool>& valid)
{
  if (valid.size() == this->num_faces() && has_tex_coords())
    valid_tex_faces_ = valid;
}


//: Label all faces with positive (counter clockwise orientation) area as valid
//  This requirement refers to the texture map coordinates
void imesh_mesh::label_ccw_tex_faces_valid()
{
  switch (tex_coord_status_)
  {
    case TEX_COORD_ON_VERT:
    {
      valid_tex_faces_.resize(this->num_faces());
      vgl_polygon<double> face(1);
      imesh_face_array_base& faces = this->faces();
      for (unsigned int f=0; f<num_faces(); ++f)
      {
        const unsigned int num_v = faces.num_verts(f);
        for (unsigned int i=0; i<num_v; ++i) {
          face.push_back(tex_coords_[faces(f,i)]);
        }
        valid_tex_faces_[f] = vgl_area_signed(face) > 0;
      }
      break;
    }
    case TEX_COORD_ON_CORNER:
      std::cerr << "imesh_mesh::label_ccw_tex_faces_valid()"
               << " not implemented for TEX_COORD_ON_CORNER\n";
      break;
    default:
      break;
  }
}

//Smart Pointer IO
void vsl_b_write(vsl_b_ostream& /*os*/, imesh_mesh_sptr&) {}
void vsl_b_write(vsl_b_ostream& /*os*/, imesh_mesh_sptr const&) {}

//: Binary load boxm scene from stream.
void vsl_b_read(vsl_b_istream& /*is*/, imesh_mesh_sptr&) {}
void vsl_b_read(vsl_b_istream& /*is*/, imesh_mesh_sptr const&) {}
