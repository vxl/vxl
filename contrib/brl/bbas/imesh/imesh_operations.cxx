// This is brl/bbas/imesh/imesh_operations.cxx
#include "imesh_operations.h"
//:
// \file

#include <vcl_iostream.h>
#include <vcl_map.h>
#include <vcl_cassert.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

//: Subdivide mesh faces into triangle
vcl_auto_ptr<imesh_regular_face_array<3> >
imesh_triangulate(const imesh_face_array_base& faces)
{
  vcl_auto_ptr<imesh_regular_face_array<3> > tris(new imesh_regular_face_array<3>);
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
vcl_auto_ptr<imesh_regular_face_array<3> >
imesh_triangulate(const imesh_regular_face_array<4>& faces)
{
  vcl_auto_ptr<imesh_regular_face_array<3> > tris(new imesh_regular_face_array<3>);
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
      vcl_auto_ptr<imesh_face_array_base> tris(
          imesh_triangulate(static_cast<const imesh_regular_face_array<4>&>(mesh.faces())));
      mesh.set_faces(tris);
      break;
    }
    default:
    {
      vcl_auto_ptr<imesh_face_array_base> tris(imesh_triangulate(mesh.faces()));
      mesh.set_faces(tris);
      break;
    }
  }
}


namespace {

void average_verts_2(imesh_vertex_array_base& verts,
                     const vcl_vector<vcl_vector<unsigned int> >& idx)
{
  assert(dynamic_cast<imesh_vertex_array<2>*>(&verts));
  imesh_vertex_array<2>& verts2 = static_cast<imesh_vertex_array<2>&>(verts);

  for (unsigned int i=0; i<idx.size(); ++i) {
    const vcl_vector<unsigned int>& new_idx = idx[i];
    imesh_vertex<2> new_v;
    for (unsigned int j=0; j<new_idx.size(); ++j) {
      const imesh_vertex<2>& v = verts2[new_idx[j]];
      new_v[0] += v[0];
      new_v[1] += v[1];
    }
    new_v[0] /= new_idx.size();
    new_v[1] /= new_idx.size();
    verts2.push_back(new_v);
  }
}


void average_verts_3(imesh_vertex_array_base& verts,
                     const vcl_vector<vcl_vector<unsigned int> >& idx)
{
  assert(dynamic_cast<imesh_vertex_array<3>*>(&verts));
  imesh_vertex_array<3>& verts3 = static_cast<imesh_vertex_array<3>&>(verts);

  for (unsigned int i=0; i<idx.size(); ++i) {
    const vcl_vector<unsigned int>& new_idx = idx[i];
    imesh_vertex<3> new_v;
    for (unsigned int j=0; j<new_idx.size(); ++j) {
      const imesh_vertex<3>& v = verts3[new_idx[j]];
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
  vcl_vector<vcl_vector<unsigned int> > merge_idx;
  // edge indices
  for (unsigned int e=0; e<num_e_verts; ++e) {
    vcl_vector<unsigned int> new_e_vert;
    new_e_vert.push_back(half_edges[2*e].vert_index());
    new_e_vert.push_back(half_edges[2*e+1].vert_index());
    merge_idx.push_back(new_e_vert);
  }
  // face indices
  for (unsigned int f=0; f<num_f_verts; ++f) {
    vcl_vector<unsigned int> new_f_vert;
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
      vcl_cerr << "can not handle vertices of dimension: "
               << verts.dim() << '\n';
      return;
  }

  if (mesh.has_tex_coords() == imesh_mesh::TEX_COORD_ON_VERT) {
    vcl_vector<vgl_point_2d<double> > tex_coords(mesh.tex_coords());
    for (unsigned int i=0; i<merge_idx.size(); ++i) {
      const vcl_vector<unsigned int>& new_idx = merge_idx[i];
      vgl_point_2d<double> new_tc(0,0);
      for (unsigned int j=0; j<new_idx.size(); ++j) {
        const vgl_point_2d<double>& v = tex_coords[new_idx[j]];
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
  vcl_auto_ptr<imesh_regular_face_array<4> > new_faces(new imesh_regular_face_array<4>);
  vcl_vector<vgl_point_2d<double> > tex_coords;
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
      vcl_cerr << "subdivision of texture coords per corner is not implemented\n";
    }
  }
  if (mesh.has_tex_coords() == imesh_mesh::TEX_COORD_ON_CORNER) {
    mesh.set_tex_coords(tex_coords);
  }


  mesh.set_faces(vcl_auto_ptr<imesh_face_array_base>(new_faces));
  mesh.remove_edge_graph();
  if (had_edges)
    mesh.build_edge_graph();
}


//: Subdivide faces into quadrilaterals (in place)
//  Add a vertex at the center of each edge
//  And a vertex at the center of each face
//  Only subdivide the selected faces
void
imesh_quad_subdivide(imesh_mesh& mesh, const vcl_set<unsigned int>& sel_faces)
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

  vcl_map<unsigned int,unsigned int> edge_map, face_map;

  // construct vector of indices of vertices to average to create new vertices
  vcl_vector<vcl_vector<unsigned int> > merge_idx;
  // edge indices
  for (unsigned int e=0; e<num_e; ++e) {
    if (sel_faces.count(half_edges[2*e].face_index()) +
       sel_faces.count(half_edges[2*e+1].face_index()) > 0) {
      vcl_vector<unsigned int> new_e_vert;
      new_e_vert.push_back(half_edges[2*e].vert_index());
      new_e_vert.push_back(half_edges[2*e+1].vert_index());
      edge_map[e] = merge_idx.size() + num_o_verts;
      merge_idx.push_back(new_e_vert);
    }
  }
  // face indices
  for (vcl_set<unsigned int>::const_iterator fi = sel_faces.begin();
       fi != sel_faces.end(); ++fi) {
    vcl_vector<unsigned int> new_f_vert;
    for (unsigned int i=0; i<faces.num_verts(*fi); ++i) {
      new_f_vert.push_back(faces(*fi,i));
    }
    face_map[*fi] = merge_idx.size() + num_o_verts;
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
      vcl_cerr << "can not handle vertices of dimension: "
               << verts.dim() << '\n';
      return;
  }

  if (mesh.has_tex_coords() == imesh_mesh::TEX_COORD_ON_VERT) {
    vcl_vector<vgl_point_2d<double> > tex_coords(mesh.tex_coords());
    for (unsigned int i=0; i<merge_idx.size(); ++i) {
      const vcl_vector<unsigned int>& new_idx = merge_idx[i];
      vgl_point_2d<double> new_tc(0,0);
      for (unsigned int j=0; j<new_idx.size(); ++j) {
        const vgl_point_2d<double>& v = tex_coords[new_idx[j]];
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
  typedef vcl_map<unsigned int,unsigned int>::const_iterator map_itr;
  typedef imesh_half_edge_set::f_const_iterator face_itr;
  vcl_auto_ptr<imesh_face_array> new_faces(new imesh_face_array);
  int group = -1;
  if (faces.has_groups())
    group = 0;
  for (unsigned int f=0; f<num_f; ++f)
  {
    map_itr i = face_map.find(f);
    if (i == face_map.end()){ // don't subdivide face
      face_itr fei = half_edges.face_begin(f);
      face_itr end = fei;
      vcl_vector<unsigned int> new_face;
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


  mesh.set_faces(vcl_auto_ptr<imesh_face_array_base>(new_faces));
  mesh.remove_edge_graph();
  if (had_edges)
    mesh.build_edge_graph();
}


//: Extract a sub-mesh containing only the faces listed in sel_faces
imesh_mesh
imesh_submesh_from_faces(const imesh_mesh& mesh, const vcl_set<unsigned int>& sel_faces)
{
  const imesh_vertex_array<3>& verts = mesh.vertices<3>();

  vcl_auto_ptr<imesh_face_array> new_faces(new imesh_face_array);
  vcl_auto_ptr<imesh_vertex_array<3> > new_verts(new imesh_vertex_array<3>);
  vcl_vector<int> vert_map(mesh.num_verts(),-1);

  unsigned int new_count = 0;
  vcl_vector<vgl_vector_3d<double> > fnormals;
  vcl_vector<vgl_vector_3d<double> > vnormals;
  vcl_vector<vgl_point_2d<double> > tex_coords;
  const vcl_vector<vcl_pair<vcl_string,unsigned int> >& groups = mesh.faces().groups();
  unsigned int group_idx = 0;
  for (vcl_set<unsigned int>::const_iterator fi=sel_faces.begin();
       fi!=sel_faces.end(); ++fi)
  {
    const unsigned int num_v = mesh.faces().num_verts(*fi);
    vcl_vector<unsigned int> new_face;
    for (unsigned int i=0; i<num_v; ++i) {
      unsigned int v = mesh.faces()(*fi,i);
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
      while (group_idx < groups.size() && groups[group_idx].second <= *fi)
      {
        new_faces->make_group(groups[group_idx].first);
        ++group_idx;
      }
    }
    new_faces->push_back(new_face);
    if (mesh.faces().has_normals())
      fnormals.push_back(mesh.faces().normal(*fi));
  }
  if (!fnormals.empty())
    new_faces->set_normals(fnormals);
  if (!vnormals.empty())
    new_verts->set_normals(vnormals);


  vcl_auto_ptr<imesh_vertex_array_base> nverts(new_verts);
  vcl_auto_ptr<imesh_face_array_base> nfaces(new_faces);
  imesh_mesh submesh(nverts,nfaces);

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
void imesh_flip_faces( imesh_mesh& mesh, const vcl_set<unsigned int>& sel_faces)
{
  for (vcl_set<unsigned int>::const_iterator fi=sel_faces.begin();
       fi!=sel_faces.end(); ++fi)
  {
    mesh.faces().flip_orientation(*fi);
  }
}


//: Compute the dual mesh using face centroids for vertices
imesh_mesh dual_mesh(const imesh_mesh& mesh)
{
  assert(mesh.has_half_edges());
  const imesh_half_edge_set& half_edges = mesh.half_edges();
  const unsigned int num_verts = mesh.num_verts();
  const unsigned int num_faces = mesh.num_faces();

  vcl_auto_ptr<imesh_face_array> new_faces(new imesh_face_array);
  vcl_auto_ptr<imesh_vertex_array<3> > new_verts(new imesh_vertex_array<3>);

  for (unsigned int i=0; i<num_faces; ++i)
  {
    vcl_vector<vgl_point_3d<double> > pts;
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
    vcl_vector<unsigned int> face;
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

  vcl_auto_ptr<imesh_face_array_base> nf(new_faces);
  vcl_auto_ptr<imesh_vertex_array_base > nv(new_verts);
  return imesh_mesh(nv,nf);
}

//: Contains a 3d point in convex polygon
bool contains_point(const imesh_mesh& mesh,vgl_point_3d<double> p)
{
  vcl_auto_ptr<imesh_face_array> new_faces(new imesh_face_array);
  vcl_auto_ptr<imesh_vertex_array<3> > new_verts(new imesh_vertex_array<3>);

  for (unsigned int i=0; i<mesh.num_faces(); ++i)
  {
    vcl_vector<vgl_point_3d<double> > pts;
    for (unsigned int j=0; j<mesh.faces().num_verts(i); ++j)
    {
      unsigned int v = mesh.faces()(i,j);
      pts.push_back(mesh.vertices<3>()[v]);
    }
    new_verts->push_back(centre(pts));
  }
  if (mesh.faces().has_normals())
    new_verts->set_normals(mesh.faces().normals());
  vcl_vector<vgl_vector_3d<double> > normals=  new_verts->normals();
  imesh_vertex_array<3>::const_iterator iter=new_verts->begin();
  unsigned int i=0;
  for (;iter!=new_verts->end();iter++,i++)
  {
    vgl_point_3d<double> new_vert((*iter)[0],(*iter)[1],(*iter)[2]);
    double dotprod=dot_product<double>(new_vert-p,normals[i]);
    if (dotprod<0)
      return false;
  }
  return true;
}
