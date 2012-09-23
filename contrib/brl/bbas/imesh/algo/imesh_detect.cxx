// This is brl/bbas/imesh/algo/imesh_detect.cxx
#include "imesh_detect.h"
//:
// \file


#include <vcl_algorithm.h>
#include <vcl_limits.h>
#include <vcl_cassert.h>
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
vcl_vector<vgl_vector_3d<double> >
sample_sphere_directions(unsigned int num_dir_samples)
{
  vcl_vector<vgl_vector_3d<double> > dirs;
  const unsigned int half_samples = num_dir_samples/2;
  for (unsigned int i=0; i<=half_samples; ++i) {
    double theta = i*vnl_math::twopi/num_dir_samples;
    double st = vcl_sin(theta);
    double ct = vcl_cos(theta);
    unsigned int num_j_samples = static_cast<unsigned int>(st*num_dir_samples+0.5);
    if (num_j_samples == 0) num_j_samples = 1;
    for (unsigned j=0; j<num_j_samples; ++j) {
      double phi = j*vnl_math::twopi/num_j_samples;
      double sp = vcl_sin(phi);
      double cp = vcl_cos(phi);
      dirs.push_back(vgl_vector_3d<double>(st*cp,st*sp,ct));
    }
  }
  return dirs;
}
// end of namespace
}


//: Return the set of triangles that are visible from this viewing direction
// Backfacing triangles are not render or counted if \a backfacing == NULL
// If \a backfacing is valid, backfacing exterior triangles are also added to this set
vcl_set<unsigned int>
imesh_detect_exterior_faces(const imesh_mesh& mesh,
                            const vgl_vector_3d<double>& dir,
                            unsigned int img_size,
                            vcl_set<unsigned int> *backfacing)
{
  assert(mesh.faces().regularity() == 3);
  assert(mesh.faces().has_normals());

  vgl_rotation_3d<double> R = make_rotation(dir);

  const imesh_vertex_array<3>& verts = mesh.vertices<3>();
  vcl_vector<vgl_point_3d<double> > pts;
  vgl_box_3d<double> box;
  for (unsigned i=0; i<verts.size(); ++i)
  {
    pts.push_back(R*vgl_point_3d<double>(verts[i]));
    box.add(pts.back());
  }
  vcl_cout << box << vcl_endl;

  vgl_vector_3d<double> t(-box.min_x(), -box.min_y(), -box.min_z());

  double max_size = vcl_max(box.width(),box.height());
  double scale = (img_size-1)/max_size;
  t *= scale;

  vcl_vector<vgl_point_2d<double> > pts_2d(pts.size());
  vcl_vector<double> depths(pts.size());
  for (unsigned i=0; i<pts.size(); ++i)
  {
    vgl_point_3d<double>& pt = pts[i];
    pt.set(pt.x()*scale,pt.y()*scale,pt.z()*scale);
    pt += t;
    pts_2d[i].set(pt.x(),pt.y());
    depths[i] = pt.z();
  }

  unsigned int ni = static_cast<unsigned int>(vcl_ceil(scale*box.max_x()+t.x()))+1;
  unsigned int nj = static_cast<unsigned int>(vcl_ceil(scale*box.max_y()+t.y()))+1;

  vil_image_view<double> depth_img(ni,nj);
  depth_img.fill(vcl_numeric_limits<double>::infinity());
  vil_image_view<unsigned int> labels(ni,nj);
  labels.fill(imesh_invalid_idx);

  const imesh_regular_face_array<3>& tris =
      static_cast<const imesh_regular_face_array<3>&>(mesh.faces());

  vcl_vector<bool> is_backfacing(tris.size(),false);
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

  vcl_set<unsigned int> ext_faces;
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
  vcl_vector<bool> ext_vert(depths.size(),false);
  unsigned int c=0;
  for (unsigned int n=0; n<depths.size(); ++n)
  {
    unsigned int i = static_cast<unsigned int>(vcl_floor(pts_2d[n].x()));
    unsigned int j = static_cast<unsigned int>(vcl_floor(pts_2d[n].y()));
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
vcl_set<unsigned int>
imesh_detect_exterior_faces(const imesh_mesh& mesh,
                            unsigned int num_dir_samples,
                            unsigned int img_size)
{
  vcl_auto_ptr<imesh_mesh> tri_mesh;
  const imesh_mesh* mesh_ptr = &mesh;
  vcl_vector<unsigned int> tri_map;
  if (mesh.faces().regularity() != 3) {
    for (unsigned int i=0; i<mesh.num_faces(); ++i)
    {
      for (unsigned int j=2; j<mesh.faces().num_verts(i); ++j)
        tri_map.push_back(i);
    }
    vcl_auto_ptr<imesh_vertex_array_base> verts_copy(mesh.vertices().clone());
    vcl_auto_ptr<imesh_face_array_base> faces_tri(imesh_triangulate(mesh.faces()));
    tri_mesh.reset(new imesh_mesh(verts_copy,faces_tri));
    tri_mesh->compute_face_normals();
    mesh_ptr = tri_mesh.get();
  }
  assert(mesh_ptr->faces().has_normals());

  vcl_vector<unsigned int> face_vote(mesh_ptr->num_faces(),0);

  vcl_vector<vgl_vector_3d<double> > dirs =
      sample_sphere_directions(num_dir_samples);

  for (unsigned int i=0; i<dirs.size(); ++i) {
    vcl_set<unsigned int> vis = imesh_detect_exterior_faces(*mesh_ptr, dirs[i], img_size);
    for (vcl_set<unsigned int>::const_iterator itr=vis.begin(); itr!=vis.end(); ++itr)
      ++face_vote[*itr];
  }

  vcl_set<unsigned int> ext;
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
                            vcl_set<unsigned int>& frontfacing,
                            vcl_set<unsigned int>& backfacing,
                            vcl_set<unsigned int>& bifacing,
                            unsigned int num_dir_samples,
                            unsigned int img_size)
{
  vcl_auto_ptr<imesh_mesh> tri_mesh;
  const imesh_mesh* mesh_ptr = &mesh;
  vcl_vector<unsigned int> tri_map;
  if (mesh.faces().regularity() != 3) {
    for (unsigned int i=0; i<mesh.num_faces(); ++i)
    {
      for (unsigned int j=2; j<mesh.faces().num_verts(i); ++j)
        tri_map.push_back(i);
    }
    vcl_auto_ptr<imesh_vertex_array_base> verts_copy(mesh.vertices().clone());
    vcl_auto_ptr<imesh_face_array_base> faces_tri(imesh_triangulate(mesh.faces()));
    tri_mesh.reset(new imesh_mesh(verts_copy,faces_tri));
    tri_mesh->compute_face_normals();
    mesh_ptr = tri_mesh.get();
  }
  assert(mesh_ptr->faces().has_normals());

  vcl_vector<unsigned int> face_vote(mesh.num_faces(),0);
  vcl_vector<unsigned int> back_face_vote(mesh.num_faces(),0);

  vcl_vector<vgl_vector_3d<double> > dirs =
      sample_sphere_directions(num_dir_samples);

  for (unsigned int i=0; i<dirs.size(); ++i) {
    vcl_set<unsigned int> back_vis;
    vcl_set<unsigned int> vis = imesh_detect_exterior_faces(*mesh_ptr, dirs[i],
                                                            img_size, &back_vis);
    for (vcl_set<unsigned int>::const_iterator itr=vis.begin();
         itr!=vis.end(); ++itr) {
      unsigned int face_ind = tri_map.empty() ? *itr : tri_map[*itr];
      ++face_vote[face_ind];
     }
    for (vcl_set<unsigned int>::const_iterator itr=back_vis.begin();
         itr!=back_vis.end(); ++itr) {
      unsigned int face_ind = tri_map.empty() ? *itr : tri_map[*itr];
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

