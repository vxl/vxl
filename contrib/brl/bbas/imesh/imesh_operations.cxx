// This is brl/bbas/imesh/imesh_operations.cxx
#include <iostream>
#include <map>
#include "imesh_operations.h"
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

//: Subdivide mesh faces into triangle
std::unique_ptr<imesh_regular_face_array<3> >
imesh_triangulate(const imesh_face_array_base& faces)
{
  std::unique_ptr<imesh_regular_face_array<3> > tris(new imesh_regular_face_array<3>);
  int group = -1;
  if (faces.has_groups())
    group = 0;
  for (unsigned int f=0; f<faces.size(); ++f) {
    for (unsigned i=2; i<faces.num_verts(f); ++i)
      tris->push_back(imesh_tri(faces(f,0),faces(f,i-1),faces(f,i)));
    if (group >= 0 && f+1 >= faces.groups()[group].second) {
      tris->make_group(faces.groups()[group++].first);
    }
  }
  return tris;
}


//: Subdivide quadrilaterals into triangle
std::unique_ptr<imesh_regular_face_array<3> >
imesh_triangulate(const imesh_regular_face_array<4>& faces)
{
  std::unique_ptr<imesh_regular_face_array<3> > tris(new imesh_regular_face_array<3>);
  int group = -1;
  if (faces.has_groups())
    group = 0;
  for (unsigned int f=0; f<faces.size(); ++f) {
    const imesh_regular_face<4>& face = faces[f];
    tris->push_back(imesh_tri(face[0],face[1],face[2]));
    tris->push_back(imesh_tri(face[0],face[2],face[3]));
    if (group >= 0 && f+1 >= faces.groups()[group].second) {
      tris->make_group(faces.groups()[group++].first);
    }
  }
  return tris;
}


//: Triangulate the faces of the mesh (in place)
void
imesh_triangulate(imesh_mesh& mesh)
{
  switch (mesh.faces().regularity())
  {
    case 3: break;
    case 4:
    {
      std::unique_ptr<imesh_face_array_base> tris(
          imesh_triangulate(static_cast<const imesh_regular_face_array<4>&>(mesh.faces())));
      mesh.set_faces(std::move(tris));
      break;
    }
    default:
    {
      std::unique_ptr<imesh_face_array_base> tris(imesh_triangulate(mesh.faces()));
      mesh.set_faces(std::move(tris));
      break;
    }
  }
}


namespace {

void average_verts_2(imesh_vertex_array_base& verts,
                     const std::vector<std::vector<unsigned int> >& idx)
{
  assert(dynamic_cast<imesh_vertex_array<2>*>(&verts));
  auto& verts2 = static_cast<imesh_vertex_array<2>&>(verts);

  for (const auto & new_idx : idx) {
    imesh_vertex<2> new_v;
    for (unsigned int j : new_idx) {
      const imesh_vertex<2>& v = verts2[j];
      new_v[0] += v[0];
      new_v[1] += v[1];
    }
    new_v[0] /= new_idx.size();
    new_v[1] /= new_idx.size();
    verts2.push_back(new_v);
  }
}


void average_verts_3(imesh_vertex_array_base& verts,
                     const std::vector<std::vector<unsigned int> >& idx)
{
  assert(dynamic_cast<imesh_vertex_array<3>*>(&verts));
  auto& verts3 = static_cast<imesh_vertex_array<3>&>(verts);

  for (const auto & new_idx : idx) {
    imesh_vertex<3> new_v;
    for (unsigned int j : new_idx) {
      const imesh_vertex<3>& v = verts3[j];
      new_v[0] += v[0];
      new_v[1] += v[1];
      new_v[2] += v[2];
    }
    new_v[0] /= new_idx.size();
    new_v[1] /= new_idx.size();
    new_v[2] /= new_idx.size();
    verts3.push_back(new_v);
  }
}
// end of namespace
};


//: Subdivide faces into quadrilaterals (in place)
//  Add a vertex at the center of each edge
//  And a vertex at the center of each face
void
imesh_quad_subdivide(imesh_mesh& mesh)
{
  bool had_edges = mesh.has_half_edges();
  if (!had_edges)
    mesh.build_edge_graph();

  const imesh_face_array_base& faces = mesh.faces();
  imesh_vertex_array_base& verts = mesh.vertices();
  const imesh_half_edge_set& half_edges = mesh.half_edges();

  const unsigned int num_o_verts = mesh.num_verts(); // original vertices
  const unsigned int num_e_verts = mesh.num_edges(); // edge vertices
  const unsigned int num_f_verts = mesh.num_faces(); // face vertices

  // construct vector of indices of vertices to average to create new vertices
  std::vector<std::vector<unsigned int> > merge_idx;
  // edge indices
  for (unsigned int e=0; e<num_e_verts; ++e) {
    std::vector<unsigned int> new_e_vert;
    new_e_vert.push_back(half_edges[2*e].vert_index());
    new_e_vert.push_back(half_edges[2*e+1].vert_index());
    merge_idx.push_back(new_e_vert);
  }
  // face indices
  for (unsigned int f=0; f<num_f_verts; ++f) {
    std::vector<unsigned int> new_f_vert;
    for (unsigned int i=0; i<faces.num_verts(f); ++i) {
      new_f_vert.push_back(faces(f,i));
    }
    merge_idx.push_back(new_f_vert);
  }

  // create and append new vertices
  switch (verts.dim()) {
    case 2:
      average_verts_2(verts,merge_idx);
      break;
    case 3:
      average_verts_3(verts,merge_idx);
      break;
    default:
      std::cerr << "can not handle vertices of dimension: "
               << verts.dim() << '\n';
      return;
  }

  if (mesh.has_tex_coords() == imesh_mesh::TEX_COORD_ON_VERT) {
    std::vector<vgl_point_2d<double> > tex_coords(mesh.tex_coords());
    for (const auto & new_idx : merge_idx) {
      vgl_point_2d<double> new_tc(0,0);
      for (unsigned int j : new_idx) {
        const vgl_point_2d<double>& v = tex_coords[j];
        new_tc.x() += v.x();
        new_tc.y() += v.y();
      }
      new_tc.x() /= new_idx.size();
      new_tc.y() /= new_idx.size();
      tex_coords.push_back(new_tc);
    }
    mesh.set_tex_coords(tex_coords);
  }

  // construct the new quad faces
  std::unique_ptr<imesh_regular_face_array<4> > new_faces(new imesh_regular_face_array<4>);
  std::vector<vgl_point_2d<double> > tex_coords;
  unsigned int f_vert_base = num_o_verts+num_e_verts;
  for (unsigned int he=0; he<half_edges.size(); ++he) {
    if (half_edges[he].is_boundary())
      continue;
    unsigned int next = half_edges[he].next_index(); // next edge
    new_faces->push_back(imesh_quad(f_vert_base + half_edges[he].face_index(), // face vertex
                                    num_o_verts + (he>>1), // current edge vertex
                                    half_edges[next].vert_index(), // original vertex
                                    num_o_verts + (next>>1) ) ); // next edge vertex
    if (mesh.has_tex_coords() == imesh_mesh::TEX_COORD_ON_CORNER) {
      std::cerr << "subdivision of texture coords per corner is not implemented\n";
    }
  }
  if (mesh.has_tex_coords() == imesh_mesh::TEX_COORD_ON_CORNER) {
    mesh.set_tex_coords(tex_coords);
  }


#if __cplusplus >= 201103L || (defined(_CPPLIB_VER) && _CPPLIB_VER >= 520)
  std::unique_ptr<imesh_face_array_base> temp( std::move(new_faces) );
  mesh.set_faces(std::move(temp));
#else
  mesh.set_faces(std::auto_ptr<imesh_face_array_base>(new_faces));
#endif
  mesh.remove_edge_graph();
  if (had_edges)
    mesh.build_edge_graph();
}


//: Subdivide faces into quadrilaterals (in place)
//  Add a vertex at the center of each edge
//  And a vertex at the center of each face
//  Only subdivide the selected faces
void
imesh_quad_subdivide(imesh_mesh& mesh, const std::set<unsigned int>& sel_faces)
{
  bool had_edges = mesh.has_half_edges();
  if (!had_edges)
    mesh.build_edge_graph();

  const imesh_face_array_base& faces = mesh.faces();
  imesh_vertex_array_base& verts = mesh.vertices();
  const imesh_half_edge_set& half_edges = mesh.half_edges();

  const unsigned int num_o_verts = mesh.num_verts(); // original vertices
  const unsigned int num_e = mesh.num_edges(); // edges
  const unsigned int num_f = mesh.num_faces(); // faces

  std::map<unsigned int,unsigned int> edge_map, face_map;

  // construct vector of indices of vertices to average to create new vertices
  std::vector<std::vector<unsigned int> > merge_idx;
  // edge indices
  for (unsigned int e=0; e<num_e; ++e) {
    if (sel_faces.count(half_edges[2*e].face_index()) +
       sel_faces.count(half_edges[2*e+1].face_index()) > 0) {
      std::vector<unsigned int> new_e_vert;
      new_e_vert.push_back(half_edges[2*e].vert_index());
      new_e_vert.push_back(half_edges[2*e+1].vert_index());
      edge_map[e] = merge_idx.size() + num_o_verts;
      merge_idx.push_back(new_e_vert);
    }
  }
  // face indices
  for (const auto & sel_face : sel_faces) {
    std::vector<unsigned int> new_f_vert;
    for (unsigned int i=0; i<faces.num_verts(sel_face); ++i) {
      new_f_vert.push_back(faces(sel_face,i));
    }
    face_map[sel_face] = merge_idx.size() + num_o_verts;
    merge_idx.push_back(new_f_vert);
  }

  // create and append new vertices
  switch (verts.dim()) {
    case 2:
      average_verts_2(verts,merge_idx);
      break;
    case 3:
      average_verts_3(verts,merge_idx);
      break;
    default:
      std::cerr << "can not handle vertices of dimension: "
               << verts.dim() << '\n';
      return;
  }

  if (mesh.has_tex_coords() == imesh_mesh::TEX_COORD_ON_VERT) {
    std::vector<vgl_point_2d<double> > tex_coords(mesh.tex_coords());
    for (const auto & new_idx : merge_idx) {
      vgl_point_2d<double> new_tc(0,0);
      for (unsigned int j : new_idx) {
        const vgl_point_2d<double>& v = tex_coords[j];
        new_tc.x() += v.x();
        new_tc.y() += v.y();
      }
      new_tc.x() /= new_idx.size();
      new_tc.y() /= new_idx.size();
      tex_coords.push_back(new_tc);
    }
    mesh.set_tex_coords(tex_coords);
  }

  // construct the new faces
  typedef std::map<unsigned int,unsigned int>::const_iterator map_itr;
  typedef imesh_half_edge_set::f_const_iterator face_itr;
  std::unique_ptr<imesh_face_array> new_faces(new imesh_face_array);
  int group = -1;
  if (faces.has_groups())
    group = 0;
  for (unsigned int f=0; f<num_f; ++f)
  {
    map_itr i = face_map.find(f);
    if (i == face_map.end()){ // don't subdivide face
      face_itr fei = half_edges.face_begin(f);
      face_itr end = fei;
      std::vector<unsigned int> new_face;
      do{
        new_face.push_back(fei->vert_index());
        map_itr i2 = edge_map.find(fei->edge_index());
        if (i2 != edge_map.end())
          new_face.push_back(i2->second);
        ++fei;
      } while (fei != end);
      new_faces->push_back(new_face);
    }
    else{ // subdivide face
      face_itr fei = half_edges.face_begin(f);
      face_itr end = fei;
      do{
        face_itr next = fei;
        ++next;
        new_faces->push_back(imesh_quad(i->second, // face vertex
                                        edge_map[fei->edge_index()], // current edge vertex
                                        next->vert_index(), // original vertex
                                        edge_map[next->edge_index()] ) ); // next edge vertex
        fei = next;
      } while (fei != end);
    }
    if (group >= 0 && f+1 >= faces.groups()[group].second) {
      new_faces->make_group(faces.groups()[group++].first);
    }
  }


#if __cplusplus >= 201103L || (defined(_CPPLIB_VER) && _CPPLIB_VER >= 520)
  std::unique_ptr<imesh_face_array_base> temp = std::move(new_faces);
  mesh.set_faces(std::move(temp));
#else
  mesh.set_faces(std::auto_ptr<imesh_face_array_base>(new_faces));
#endif
  mesh.remove_edge_graph();
  if (had_edges)
    mesh.build_edge_graph();
}


//: Extract a sub-mesh containing only the faces listed in sel_faces
imesh_mesh
imesh_submesh_from_faces(const imesh_mesh& mesh, const std::set<unsigned int>& sel_faces)
{
  const imesh_vertex_array<3>& verts = mesh.vertices<3>();

  std::unique_ptr<imesh_face_array> new_faces(new imesh_face_array);
  std::unique_ptr<imesh_vertex_array<3> > new_verts(new imesh_vertex_array<3>);
  std::vector<int> vert_map(mesh.num_verts(),-1);

  unsigned int new_count = 0;
  std::vector<vgl_vector_3d<double> > fnormals;
  std::vector<vgl_vector_3d<double> > vnormals;
  std::vector<vgl_point_2d<double> > tex_coords;
  const std::vector<std::pair<std::string,unsigned int> >& groups = mesh.faces().groups();
  unsigned int group_idx = 0;
  for (const auto & sel_face : sel_faces)
  {
    const unsigned int num_v = mesh.faces().num_verts(sel_face);
    std::vector<unsigned int> new_face;
    for (unsigned int i=0; i<num_v; ++i) {
      unsigned int v = mesh.faces()(sel_face,i);
      int& nv = vert_map[v];
      if (nv < 0) {
        nv = new_count++;
        new_verts->push_back(verts[v]);
        if (verts.has_normals())
          vnormals.push_back(verts.normal(v));
        if (mesh.has_tex_coords() == imesh_mesh::TEX_COORD_ON_VERT)
          tex_coords.push_back(mesh.tex_coords()[v]);
      }
      new_face.push_back(nv);
    }
    if (mesh.faces().has_groups()) {
      while (group_idx < groups.size() && groups[group_idx].second <= sel_face)
      {
        new_faces->make_group(groups[group_idx].first);
        ++group_idx;
      }
    }
    new_faces->push_back(new_face);
    if (mesh.faces().has_normals())
      fnormals.push_back(mesh.faces().normal(sel_face));
  }
  if (!fnormals.empty())
    new_faces->set_normals(fnormals);
  if (!vnormals.empty())
    new_verts->set_normals(vnormals);


  std::unique_ptr<imesh_vertex_array_base> nverts(std::move(new_verts));
  std::unique_ptr<imesh_face_array_base> nfaces(std::move(new_faces));
  imesh_mesh submesh(std::move(nverts),std::move(nfaces));

  if (mesh.has_tex_coords())
    submesh.set_tex_coords(tex_coords);

  return submesh;
}


//: Flip the orientation of all mesh faces
void imesh_flip_faces( imesh_mesh& mesh )
{
  for (unsigned int f=0; f<mesh.num_faces(); ++f)
  {
    mesh.faces().flip_orientation(f);
  }
}


//: Flip the orientation of the selected faces
void imesh_flip_faces( imesh_mesh& mesh, const std::set<unsigned int>& sel_faces)
{
  for ( const auto & sel_face : sel_faces)
  {
    mesh.faces().flip_orientation(sel_face);
  }
}


//: Compute the dual mesh using face centroids for vertices
imesh_mesh dual_mesh(const imesh_mesh& mesh)
{
  assert(mesh.has_half_edges());
  const imesh_half_edge_set& half_edges = mesh.half_edges();
  const unsigned int num_verts = mesh.num_verts();
  const unsigned int num_faces = mesh.num_faces();

  std::unique_ptr<imesh_face_array> new_faces(new imesh_face_array);
  std::unique_ptr<imesh_vertex_array<3> > new_verts(new imesh_vertex_array<3>);

  for (unsigned int i=0; i<num_faces; ++i)
  {
    std::vector<vgl_point_3d<double> > pts;
    for (unsigned int j=0; j<mesh.faces().num_verts(i); ++j)
    {
      unsigned int v = mesh.faces()(i,j);
      pts.push_back(mesh.vertices<3>()[v]);
    }
    new_verts->push_back(centre(pts));
  }
  if (mesh.faces().has_normals())
    new_verts->set_normals(mesh.faces().normals());

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
  if (mesh.vertices().has_normals())
    new_faces->set_normals(mesh.vertices().normals());

  std::unique_ptr<imesh_face_array_base> nf(std::move(new_faces));
  std::unique_ptr<imesh_vertex_array_base > nv(std::move(new_verts));
  return imesh_mesh(std::move(nv),std::move(nf));
}

//: Contains a 3d point in convex polygon
bool contains_point(const imesh_mesh& mesh,vgl_point_3d<double> p)
{
  std::unique_ptr<imesh_face_array> new_faces(new imesh_face_array);
  std::unique_ptr<imesh_vertex_array<3> > new_verts(new imesh_vertex_array<3>);

  for (unsigned int i=0; i<mesh.num_faces(); ++i)
  {
    std::vector<vgl_point_3d<double> > pts;
    for (unsigned int j=0; j<mesh.faces().num_verts(i); ++j)
    {
      unsigned int v = mesh.faces()(i,j);
      pts.push_back(mesh.vertices<3>()[v]);
    }
    new_verts->push_back(centre(pts));
  }
  if (mesh.faces().has_normals())
    new_verts->set_normals(mesh.faces().normals());
  std::vector<vgl_vector_3d<double> > normals=  new_verts->normals();
  imesh_vertex_array<3>::const_iterator iter=new_verts->begin();
  unsigned int i=0;
  for (;iter!=new_verts->end();iter++,i++)
  {
    vgl_point_3d<double> new_vert((*iter)[0],(*iter)[1],(*iter)[2]);
    auto dotprod=dot_product<double>(new_vert-p,normals[i]);
    if (dotprod<0)
      return false;
  }
  return true;
}
