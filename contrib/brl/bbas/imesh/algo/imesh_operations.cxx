// This is brl/bbas/imesh/algo/imesh_operations.cxx
#include <list>
#include <iostream>
#include "imesh_operations.h"
//:
// \file

#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_area.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_triangle_test.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_double_3.h>
#include <vnl/algo/vnl_svd.h>
#include <cassert>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: Compute the dual mesh using vertex normals to compute dual vertices
imesh_mesh dual_mesh_with_normals(const imesh_mesh& mesh,
                                  const imesh_vertex_array_base& old_verts,
                                  double tau)
{
  assert(mesh.has_half_edges());
  const imesh_half_edge_set& half_edges = mesh.half_edges();
  const auto& init_verts =
      static_cast<const imesh_vertex_array<3>&>(old_verts);
  const unsigned int num_verts = mesh.num_verts();
  const unsigned int num_faces = mesh.num_faces();

  std::unique_ptr<imesh_face_array> new_faces(new imesh_face_array);
  std::unique_ptr<imesh_vertex_array<3> > new_verts(new imesh_vertex_array<3>);

  for (unsigned int i=0; i<num_verts; ++i)
  {
    typedef imesh_half_edge_set::v_const_iterator vitr;
    std::vector<unsigned int> face;
    vitr end = half_edges.vertex_begin(i), v = end;
    face.push_back(v->face_index());
    for (++v; v != end; ++v)
    {
      face.push_back(v->face_index());
    }
    new_faces->push_back(face);
  }
  new_faces->set_normals(mesh.vertices().normals());


  for (unsigned int i=0; i<num_faces; ++i)
  {
    const unsigned int num_planes = mesh.faces().num_verts(i);
    vgl_point_3d<double> p0(init_verts[i]);
    vgl_vector_3d<double> v0(p0.x(), p0.y(), p0.z());
    vnl_matrix<double> A(num_planes, 3);
    vnl_vector<double> b(num_planes);
    for (unsigned int j=0; j<num_planes; ++j)
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

  std::unique_ptr<imesh_face_array_base> nf(std::move(new_faces));
  std::unique_ptr<imesh_vertex_array_base > nv(std::move(new_verts));
  return imesh_mesh(std::move(nv),std::move(nf));
}


//: triangulate the 2-d polygon and add triangles to \a tris
void imesh_triangulate_face(const std::vector<vgl_point_2d<double> >& face_v,
                            const std::vector<unsigned int>& face_i,
                            imesh_regular_face_array<3>& tris)
{
  const unsigned int numv = face_v.size();
  bool ccw = vgl_area_signed(vgl_polygon<double>(face_v)) > 0;
  std::vector<bool> concave_vert(numv,false);
  std::list<unsigned int> remain;
  // determine the concavity of each polygon vertex
  for (unsigned int i1=numv-2, i2=numv-1, i3=0; i3<numv; i1=i2, i2=i3++)
  {
    concave_vert[i2] = ccw != (signed_angle(face_v[i2]-face_v[i1],
                                            face_v[i3]-face_v[i2]) > 0);
    remain.push_back(i3);
  }

  unsigned int remain_size = 0;
  while (remain.size() > 2 && remain_size != remain.size()) {
    remain_size = remain.size();
    auto curr = remain.end(), prev = --curr;
    --prev;
    for (auto next=remain.begin(); next!=remain.end(); prev=curr, curr=next++)
    {
      if (concave_vert[*curr])
        continue;

      // test for an ear (a triangle completely contained in the polygon)
      bool inside = false;
      for (auto itr=remain.begin(); itr!=remain.end(); ++itr)
      {
        if (!concave_vert[*itr] || itr==curr || itr==prev || itr==next)
          continue;
        inside = vgl_triangle_test_inside(face_v[*prev].x(),face_v[*prev].y(),
                                          face_v[*curr].x(),face_v[*curr].y(),
                                          face_v[*next].x(),face_v[*next].y(),
                                          face_v[*itr ].x(),face_v[*itr ].y());
        if (inside)
          break;
      }
      if (inside)
        continue;

      //found an ear, remove it
      tris.push_back(imesh_tri(face_i[*prev],face_i[*curr],face_i[*next]));
      remain.erase(curr);
      if (remain.size() < 3)
        break;

      // get the indices before previous and after next
      auto pprev = prev;
      if (pprev == remain.begin())
        pprev = remain.end();
      --pprev;
      auto nnext = next;
      ++nnext;
      if (nnext == remain.end())
        nnext = remain.begin();
      // update the concavities
      vgl_vector_2d<double> v1 = face_v[*prev]-face_v[*pprev];
      vgl_vector_2d<double> v2 = face_v[*next]-face_v[*prev];
      vgl_vector_2d<double> v3 = face_v[*nnext]-face_v[*next];
      concave_vert[*prev] = ccw != (signed_angle(v1,v2) > 0);
      concave_vert[*next] = ccw != (signed_angle(v2,v3) > 0);
      curr = prev;
      prev = pprev;
    }
  }

  // This case should never happen
  if (remain_size == remain.size())
    std::cout << "error: "<<remain.size()<<" vertices remaining and no more ears"<<std::endl;
}


//: Triangulate the faces of the mesh (in place)
//  Uses mesh geometry to handle nonconvex faces
void
imesh_triangulate_nonconvex(imesh_mesh& mesh)
{
  const imesh_face_array_base& faces = mesh.faces();
  assert(mesh.vertices().dim() == 3);
  const imesh_vertex_array<3>& verts = mesh.vertices<3>();

  std::unique_ptr<imesh_face_array_base> tris_base(new imesh_regular_face_array<3>);
  auto* tris =
      static_cast<imesh_regular_face_array<3>*> (tris_base.get());
  int group = -1;
  if (faces.has_groups())
    group = 0;
  for (unsigned int f=0; f<faces.size(); ++f) {
    const unsigned int numv = faces.num_verts(f);
    if (numv == 3)
    {
      tris->push_back(imesh_tri(faces(f,0),faces(f,1),faces(f,2)));
      continue;
    }

    // find the best planar projection of the face
    // to apply 2-d nonconvex polygon triangulation
    vnl_matrix<double> M(3,numv);
    vnl_vector<double> mean(3,0.0);
    for (unsigned i=0; i<numv; ++i)
    {
      const imesh_vertex<3>& v = verts[faces(f,i)];
      mean[0] += M(0,i) = v[0];
      mean[1] += M(1,i) = v[1];
      mean[2] += M(2,i) = v[2];
    }
    mean /= numv;
    for (unsigned i=0; i<numv; ++i)
    {
      M(0,i) -= mean[0];
      M(1,i) -= mean[1];
      M(2,i) -= mean[2];
    }
    vnl_svd<double> svd_M(M);
    vnl_matrix<double> P = svd_M.U().extract(3,2).transpose();

    std::vector<vgl_point_2d<double> > face_v;
    std::vector<unsigned int> face_i;
    for (unsigned i=0; i<numv; ++i)
    {
      const imesh_vertex<3>& v = verts[faces(f,i)];
      vnl_vector<double> p3 = vnl_double_3(v[0],v[1],v[2])-mean;
      vnl_vector<double> p2 = P*p3;
      face_v.emplace_back(p2[0],p2[1]);
      face_i.push_back(faces(f,i));
    }
    imesh_triangulate_face(face_v, face_i, *tris);
    if (group >= 0 && f+1 >= faces.groups()[group].second) {
      tris->make_group(faces.groups()[group++].first);
    }
  }

  mesh.set_faces(std::move(tris_base));
}
