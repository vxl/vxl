// This is brl/bbas/imesh/algo/imesh_operations.cxx

//:
// \file


#include "imesh_operations.h"
#include <vgl/vgl_plane_3d.h>
#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_svd.h>



//: Compute the dual mesh using vertex normals to compute dual vertices 
imesh_mesh dual_mesh_with_normals(const imesh_mesh& mesh,
                                  const imesh_vertex_array_base& old_verts,
                                  double tau)
{
  assert(mesh.has_half_edges());
  const imesh_half_edge_set& half_edges = mesh.half_edges();
  const imesh_vertex_array<3>& init_verts =
      static_cast<const imesh_vertex_array<3>&>(old_verts);
  const unsigned int num_verts = mesh.num_verts();
  const unsigned int num_faces = mesh.num_faces();

  vcl_auto_ptr<imesh_face_array> new_faces(new imesh_face_array);
  vcl_auto_ptr<imesh_vertex_array<3> > new_verts(new imesh_vertex_array<3>);

  for(unsigned int i=0; i<num_verts; ++i)
  {
    typedef imesh_half_edge_set::v_const_iterator vitr;
    vcl_vector<unsigned int> face;
    vitr end = half_edges.vertex_begin(i), v = end;
    face.push_back(v->face_index());
    for(++v; v != end; ++v)
    {
      face.push_back(v->face_index());
    }
    new_faces->push_back(face);
  }
  new_faces->set_normals(mesh.vertices().normals());


  for(unsigned int i=0; i<num_faces; ++i)
  {
    const unsigned int num_planes = mesh.faces().num_verts(i);
    vgl_point_3d<double> p0(init_verts[i]);
    vgl_vector_3d<double> v0(p0.x(), p0.y(), p0.z());
    vnl_matrix<double> A(num_planes, 3);
    vnl_vector<double> b(num_planes);
    for(unsigned int j=0; j<num_planes; ++j)
    {
      unsigned int v = mesh.faces()(i,j);
      vgl_point_3d<double> c(mesh.vertices<3>()[v]);
      c -= v0;
      vgl_plane_3d<double> p(mesh.vertices().normal(v), c);
      A(j,0) = p.nx();
      A(j,1) = p.ny();
      A(j,2) = p.nz();
      b(j)   = -p.d();
    }

    vnl_svd<double> svd(A);
    svd.zero_out_relative(tau);
    vgl_point_3d<double> p1(svd.solve(b).begin());
    p1 += v0;
    new_verts->push_back(p1);
  }

  vcl_auto_ptr<imesh_face_array_base> nf(new_faces);
  vcl_auto_ptr<imesh_vertex_array_base > nv(new_verts);
  return imesh_mesh(nv,nf);
}
