// This is brl/bbas/imesh/algo/imesh_detect.cxx
#include <iostream>
#include <algorithm>
#include <limits>
#include "imesh_detect.h"
//:
// \file


#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vil/vil_image_view.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3.h>
#include <imesh/algo/imesh_project.h>
#include <imesh/algo/imesh_render.h>
#include <imesh/imesh_operations.h>

namespace{

vgl_rotation_3d<double>
make_rotation(const vgl_vector_3d<double>& dir)
{
  vgl_vector_3d<double> z(normalized(dir));
  vgl_vector_3d<double> x = cross_product(vgl_vector_3d<double>(0,0,-1),z);
  if (x.sqr_length() == 0.0)
     x = cross_product(vgl_vector_3d<double>(0,-1,0),z);
  normalize(x);
  vgl_vector_3d<double> y = cross_product(z,x);

  vnl_double_3x3 A;
  A.set_row(0,vnl_double_3(x.x(), x.y(), x.z()));
  A.set_row(1,vnl_double_3(y.x(), y.y(), y.z()));
  A.set_row(2,vnl_double_3(z.x(), z.y(), z.z()));
  return vgl_rotation_3d<double>(A);
}

//: generate sample viewing directions from the sphere
std::vector<vgl_vector_3d<double> >
sample_sphere_directions(unsigned int num_dir_samples)
{
  std::vector<vgl_vector_3d<double> > dirs;
  const unsigned int half_samples = num_dir_samples/2;
  for (unsigned int i=0; i<=half_samples; ++i) {
    double theta = i*vnl_math::twopi/num_dir_samples;
    double st = std::sin(theta);
    double ct = std::cos(theta);
    auto num_j_samples = static_cast<unsigned int>(st*num_dir_samples+0.5);
    if (num_j_samples == 0) num_j_samples = 1;
    for (unsigned j=0; j<num_j_samples; ++j) {
      double phi = j*vnl_math::twopi/num_j_samples;
      double sp = std::sin(phi);
      double cp = std::cos(phi);
      dirs.emplace_back(st*cp,st*sp,ct);
    }
  }
  return dirs;
}
// end of namespace
}


//: Return the set of triangles that are visible from this viewing direction
// Backfacing triangles are not render or counted if \a backfacing == NULL
// If \a backfacing is valid, backfacing exterior triangles are also added to this set
std::set<unsigned int>
imesh_detect_exterior_faces(const imesh_mesh& mesh,
                            const vgl_vector_3d<double>& dir,
                            unsigned int img_size,
                            std::set<unsigned int> *backfacing)
{
  assert(mesh.faces().regularity() == 3);
  assert(mesh.faces().has_normals());

  vgl_rotation_3d<double> R = make_rotation(dir);

  const imesh_vertex_array<3>& verts = mesh.vertices<3>();
  std::vector<vgl_point_3d<double> > pts;
  vgl_box_3d<double> box;
  for (const auto & vert : verts)
  {
    pts.push_back(R*vgl_point_3d<double>(vert));
    box.add(pts.back());
  }
  std::cout << box << std::endl;

  vgl_vector_3d<double> t(-box.min_x(), -box.min_y(), -box.min_z());

  double max_size = std::max(box.width(),box.height());
  double scale = (img_size-1)/max_size;
  t *= scale;

  std::vector<vgl_point_2d<double> > pts_2d(pts.size());
  std::vector<double> depths(pts.size());
  for (unsigned i=0; i<pts.size(); ++i)
  {
    vgl_point_3d<double>& pt = pts[i];
    pt.set(pt.x()*scale,pt.y()*scale,pt.z()*scale);
    pt += t;
    pts_2d[i].set(pt.x(),pt.y());
    depths[i] = pt.z();
  }

  unsigned int ni = static_cast<unsigned int>(std::ceil(scale*box.max_x()+t.x()))+1;
  unsigned int nj = static_cast<unsigned int>(std::ceil(scale*box.max_y()+t.y()))+1;

  vil_image_view<double> depth_img(ni,nj);
  depth_img.fill(std::numeric_limits<double>::infinity());
  vil_image_view<unsigned int> labels(ni,nj);
  labels.fill(imesh_invalid_idx);

  const auto& tris =
      static_cast<const imesh_regular_face_array<3>&>(mesh.faces());

  std::vector<bool> is_backfacing(tris.size(),false);
  for (unsigned i=0; i<tris.size(); ++i) {
    const imesh_regular_face<3>& tri = tris[i];
    is_backfacing[i] = dot_product(tris.normal(i),dir) > 0;
    if (!backfacing && is_backfacing[i])
      continue;
    imesh_render_triangle_label(pts[tri[0]],
                                pts[tri[1]],
                                pts[tri[2]],
                                i,
                                labels,
                                depth_img);
  }

  std::set<unsigned int> ext_faces;
  const unsigned int num_pixels = labels.ni()*labels.nj();
  const unsigned int* pixel = labels.top_left_ptr();
  for (unsigned int i=0; i<num_pixels; ++pixel, ++i)
  {
    if (*pixel != imesh_invalid_idx) {
      if (backfacing && is_backfacing[*pixel])
        backfacing->insert(*pixel);
      else
        ext_faces.insert(*pixel);
    }
  }

#if 0
  std::vector<bool> ext_vert(depths.size(),false);
  unsigned int c=0;
  for (unsigned int n=0; n<depths.size(); ++n)
  {
    unsigned int i = static_cast<unsigned int>(std::floor(pts_2d[n].x()));
    unsigned int j = static_cast<unsigned int>(std::floor(pts_2d[n].y()));
    if (depths[n] <= depth_img(i,j)+1.0) {
      ext_vert[n] = true;
      ++c;
    }
  }

  for (unsigned int i=0; i<tris.size(); ++i)
  {
    const imesh_regular_face<3>& tri = tris[i];
    if (ext_vert[tri[0]] && ext_vert[tri[1]] && ext_vert[tri[2]])
      ext_faces.insert(i);
  }
#endif

  return ext_faces;
}


//: Return the set of triangles that are visible in some of the many sample view directions
std::set<unsigned int>
imesh_detect_exterior_faces(const imesh_mesh& mesh,
                            unsigned int num_dir_samples,
                            unsigned int img_size)
{
  std::unique_ptr<imesh_mesh> tri_mesh;
  const imesh_mesh* mesh_ptr = &mesh;
  std::vector<unsigned int> tri_map;
  if (mesh.faces().regularity() != 3) {
    for (unsigned int i=0; i<mesh.num_faces(); ++i)
    {
      for (unsigned int j=2; j<mesh.faces().num_verts(i); ++j)
        tri_map.push_back(i);
    }
    std::unique_ptr<imesh_vertex_array_base> verts_copy(mesh.vertices().clone());
    std::unique_ptr<imesh_face_array_base> faces_tri(imesh_triangulate(mesh.faces()));
    tri_mesh.reset(new imesh_mesh(std::move(verts_copy),std::move(faces_tri)));
    tri_mesh->compute_face_normals();
    mesh_ptr = tri_mesh.get();
  }
  assert(mesh_ptr->faces().has_normals());

  std::vector<unsigned int> face_vote(mesh_ptr->num_faces(),0);

  std::vector<vgl_vector_3d<double> > dirs =
      sample_sphere_directions(num_dir_samples);

  for (const auto & dir : dirs) {
    std::set<unsigned int> vis = imesh_detect_exterior_faces(*mesh_ptr, dir, img_size);
    for (const auto & vi : vis)
      ++face_vote[vi];
  }

  std::set<unsigned int> ext;
  for (unsigned int i=0; i<face_vote.size(); ++i) {
    if (double(face_vote[i])/dirs.size() > 0.0) {
      if (tri_map.empty())
        ext.insert(i);
      else
        ext.insert(tri_map[i]);
    }
  }
  return ext;
}


//: Return the set of triangles that are visible in some of the many sample view directions
//  Does render backfacing faces and classifies exterior faces as:
//  - frontfacing - seen only from the front
//  - backfacing  - seen only from the back
//  - bifacing    - seen from both sides
void
imesh_detect_exterior_faces(const imesh_mesh& mesh,
                            std::set<unsigned int>& frontfacing,
                            std::set<unsigned int>& backfacing,
                            std::set<unsigned int>& bifacing,
                            unsigned int num_dir_samples,
                            unsigned int img_size)
{
  std::unique_ptr<imesh_mesh> tri_mesh;
  const imesh_mesh* mesh_ptr = &mesh;
  std::vector<unsigned int> tri_map;
  if (mesh.faces().regularity() != 3) {
    for (unsigned int i=0; i<mesh.num_faces(); ++i)
    {
      for (unsigned int j=2; j<mesh.faces().num_verts(i); ++j)
        tri_map.push_back(i);
    }
    std::unique_ptr<imesh_vertex_array_base> verts_copy(mesh.vertices().clone());
    std::unique_ptr<imesh_face_array_base> faces_tri(imesh_triangulate(mesh.faces()));
    tri_mesh.reset(new imesh_mesh(std::move(verts_copy),std::move(faces_tri)));
    tri_mesh->compute_face_normals();
    mesh_ptr = tri_mesh.get();
  }
  assert(mesh_ptr->faces().has_normals());

  std::vector<unsigned int> face_vote(mesh.num_faces(),0);
  std::vector<unsigned int> back_face_vote(mesh.num_faces(),0);

  std::vector<vgl_vector_3d<double> > dirs =
      sample_sphere_directions(num_dir_samples);

  for (const auto & dir : dirs) {
    std::set<unsigned int> back_vis;
    std::set<unsigned int> vis = imesh_detect_exterior_faces(*mesh_ptr, dir,
                                                            img_size, &back_vis);
    for (const auto & vi : vis) {
      unsigned int face_ind = tri_map.empty() ? vi : tri_map[vi];
      ++face_vote[face_ind];
     }
    for (const auto & back_vi : back_vis) {
      unsigned int face_ind = tri_map.empty() ? back_vi : tri_map[back_vi];
      ++back_face_vote[face_ind];
    }
  }

  frontfacing.clear();
  backfacing.clear();
  bifacing.clear();
  for (unsigned int i=0; i<face_vote.size(); ++i) {
    if (face_vote[i] > 0) {
      if (back_face_vote[i] > 0)
        bifacing.insert(i);
      else
        frontfacing.insert(i);
    }
    else if (back_face_vote[i] > 0)
      backfacing.insert(i);
  }
}
