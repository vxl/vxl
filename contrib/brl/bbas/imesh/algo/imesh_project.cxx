// This is brl/bbas/imesh/algo/imesh_project.cxx
#include <iostream>
#include <algorithm>
#include <limits>
#include <cmath>
#include "imesh_project.h"
//:
// \file


#include <imesh/imesh_operations.h>
#include <imesh/algo/imesh_intersect.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_line_3d_2_points.h>
#include <vgl/vgl_triangle_test.h>
#include <vgl/vgl_intersection.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_triangle_scan_iterator.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>


void
imesh_project_verts(const imesh_vertex_array<3>& verts3d,
                    const vpgl_proj_camera<double>& camera,
                    std::vector<vgl_point_2d<double> >& verts2d)
{
  verts2d.resize(verts3d.size());
  for (unsigned int i=0; i<verts3d.size(); ++i)
    verts2d[i] = camera.project(verts3d[i]);
}

void
imesh_project_verts(const std::vector<vgl_point_3d<double> >& verts3d,
                    const vpgl_proj_camera<double>& camera,
                    std::vector<vgl_point_2d<double> >& verts2d)
{
  verts2d.resize(verts3d.size());
  for (unsigned int i=0; i<verts3d.size(); ++i)
    verts2d[i] = camera.project(verts3d[i]);
}


void
imesh_project_verts(const std::vector<vgl_point_3d<double> >& verts3d,
                    const vpgl_proj_camera<double>& camera,
                    std::vector<vgl_point_2d<double> >& verts2d,
                    std::vector<double>& depths)
{
  verts2d.resize(verts3d.size());
  depths.resize(verts3d.size());
  for (unsigned int i=0; i<verts3d.size(); ++i) {
    vgl_homg_point_2d<double> pt = camera.project(verts3d[i]);
    verts2d[i] = pt;
    depths[i] = pt.w();
  }
}


void
imesh_project_verts(const imesh_vertex_array<3>& verts3d,
                    const vpgl_proj_camera<double>& camera,
                    std::vector<vgl_point_2d<double> >& verts2d,
                    std::vector<double>& depths)
{
  verts2d.resize(verts3d.size());
  depths.resize(verts3d.size());
  for (unsigned int i=0; i<verts3d.size(); ++i) {
    vgl_homg_point_2d<double> pt = camera.project(verts3d[i]);
    verts2d[i] = pt;
    depths[i] = pt.w();
  }
}


void
imesh_distort_verts(const std::vector<vgl_point_2d<double> >& in_verts,
                    const vpgl_lens_distortion<double>& lens,
                    std::vector<vgl_point_2d<double> >& out_verts)
{
  out_verts.resize(in_verts.size());
  for (unsigned int i=0; i<in_verts.size(); ++i)
    out_verts[i] = lens.undistort(vgl_homg_point_2d<double>(in_verts[i]));
}


//: Render a triangle defined by its vertices
void imesh_render_triangle(const vgl_point_2d<double>& v1,
                           const vgl_point_2d<double>& v2,
                           const vgl_point_2d<double>& v3,
                           vil_image_view<bool>& image)
{
  vgl_triangle_scan_iterator<double> tsi;
  tsi.a.x = v1.x();  tsi.a.y = v1.y();
  tsi.b.x = v2.x();  tsi.b.y = v2.y();
  tsi.c.x = v3.x();  tsi.c.y = v3.y();
  for (tsi.reset(); tsi.next(); ) {
    int y = tsi.scany();
    if (y<0 || y>=int(image.nj())) continue;
    int min_x = tsi.startx();
    int max_x = tsi.endx();
    if (min_x >= (int)image.ni() || max_x < 0)
      continue;
    if (min_x < 0) min_x = 0;
    if (max_x >= (int)image.ni()) max_x = image.ni()-1;
    for (int x = min_x; x <= max_x; ++x) {
      image(x,y) = true;
    }
  }
}


//: Render a triangle defined by its vertices
void imesh_render_triangle_interp(const vgl_point_2d<double>& v1,
                                  const vgl_point_2d<double>& v2,
                                  const vgl_point_2d<double>& v3,
                                  const double& i1, const double& i2, const double& i3,
                                  vil_image_view<double>& image)
{
  vgl_triangle_scan_iterator<double> tsi;
  tsi.a.x= v1.x();  tsi.a.y = v1.y();
  tsi.b.x = v2.x();  tsi.b.y = v2.y();
  tsi.c.x = v3.x();  tsi.c.y = v3.y();
  vgl_vector_3d<double> b1(v2.x()-v1.x(), v2.y()-v1.y(), i2-i1);
  vgl_vector_3d<double> b2(v3.x()-v1.x(), v3.y()-v1.y(), i3-i1);
  vgl_vector_3d<double> n = cross_product(b1,b2);
  double A = -n.x()/n.z();
  double B = -n.y()/n.z();
  double C = (v1.x()*n.x() + v1.y()*n.y() + i1*n.z())/n.z();
  for (tsi.reset(); tsi.next(); ) {
    int y = tsi.scany();
    if (y<0 || y>=int(image.nj())) continue;
    int min_x = tsi.startx();
    int max_x = tsi.endx();
    if (min_x >= (int)image.ni() || max_x < 0)
      continue;
    if (min_x < 0) min_x = 0;
    if (max_x >= (int)image.ni()) max_x = image.ni()-1;
    double new_i = B*y+C;
    for (int x = min_x; x <= max_x; ++x) {
      double depth = new_i + A*x;
      if (depth < image(x,y))
        image(x,y) = depth;
    }
  }
}


void
imesh_render_triangles(const imesh_regular_face_array<3>& tris,
                       const std::vector<vgl_point_2d<double> >& img_verts,
                       vil_image_view<bool>& image)
{
  for (unsigned i=0; i<tris.size(); ++i) {
    imesh_render_triangle(img_verts[tris(i,0)],
                          img_verts[tris(i,1)],
                          img_verts[tris(i,2)],
                          image);
  }
}


void
imesh_render_faces(const imesh_mesh& mesh,
                   const std::vector<vgl_point_2d<double> >& img_verts,
                   vil_image_view<bool>& image)
{
  const imesh_face_array_base& faces = mesh.faces();
  std::unique_ptr<imesh_regular_face_array<3> > tri_data;
  const imesh_regular_face_array<3>* tris;
  if (faces.regularity() != 3) {
    tri_data = imesh_triangulate(faces);
    tris = tri_data.get();
  }
  else {
    tris = static_cast<const imesh_regular_face_array<3>*>(&faces);
  }
  imesh_render_triangles(*tris,img_verts,image);
}


void
imesh_render_triangles_interp(const imesh_regular_face_array<3>& tris,
                              const std::vector<vgl_point_2d<double> >& img_verts,
                              const std::vector<double>& vals,
                              vil_image_view<double>& image)
{
  for (auto tri : tris) {
    imesh_render_triangle_interp(img_verts[tri[0]],
                                 img_verts[tri[1]],
                                 img_verts[tri[2]],
                                 vals[tri[0]],
                                 vals[tri[1]],
                                 vals[tri[2]],
                                 image);
  }
}

void
imesh_render_faces_interp(const imesh_mesh& mesh,
                          const std::vector<vgl_point_2d<double> >& img_verts,
                          const std::vector<double>& vals,
                          vil_image_view<double>& image)
{
  const imesh_face_array_base& faces = mesh.faces();
  std::unique_ptr<imesh_regular_face_array<3> > tri_data;
  const imesh_regular_face_array<3>* tris;
  if (faces.regularity() != 3) {
    tri_data = imesh_triangulate(faces);
    tris = tri_data.get();
  }
  else {
    tris = static_cast<const imesh_regular_face_array<3>*>(&faces);
  }
  imesh_render_triangles_interp(*tris,img_verts,vals,image);
}


//: project the mesh onto the image plane using the camera and lens distortion
//  Set each pixel of the image to true if the mesh project onto it
void imesh_project(const imesh_mesh& mesh,
                   const vpgl_proj_camera<double>& camera,
                   const vpgl_lens_distortion<double>& lens,
                   vil_image_view<bool>& image)
{
  assert(mesh.vertices().dim() == 3);
  const auto& verts3d =
      static_cast<const imesh_vertex_array<3>&>(mesh.vertices());
  std::vector<vgl_point_2d<double> > verts2d;
  imesh_project_verts(verts3d, camera, verts2d);
  imesh_distort_verts(verts2d, lens, verts2d);
  imesh_render_faces(mesh, verts2d, image);
}


//: project the front-facing triangles of the mesh onto the image plane
//  Using the camera and lens distortion
//  Set each pixel of the image to true if the mesh projects onto it
void imesh_project(const imesh_mesh& mesh,
                   const std::vector<vgl_vector_3d<double> >& normals,
                   const vpgl_proj_camera<double>& camera,
                   const vpgl_lens_distortion<double>& lens,
                   vil_image_view<bool>& image,
                   vgl_box_2d<unsigned int>* bbox)
{
  const imesh_face_array_base& faces = mesh.faces();
  std::unique_ptr<imesh_regular_face_array<3> > tri_data;
  const imesh_regular_face_array<3>* tri_ptr;
  if (faces.regularity() != 3) {
    tri_data = imesh_triangulate(faces);
    tri_ptr = tri_data.get();
  }
  else {
    tri_ptr = static_cast<const imesh_regular_face_array<3>*>(&faces);
  }
  const imesh_regular_face_array<3>& tris = *tri_ptr;
  const imesh_vertex_array<3>& verts3d = mesh.vertices<3>();

  std::vector<vgl_point_2d<double> > verts2d;
  imesh_project_verts(verts3d, camera, verts2d);
  imesh_distort_verts(verts2d, lens, verts2d);

  if (bbox) {
    bbox->set_min_x(0);  bbox->set_min_y(0);
    bbox->set_max_x(image.ni()-1);  bbox->set_max_y(image.nj()-1);
    imesh_projection_bounds(verts2d,*bbox);
  }

  vgl_homg_point_3d<double> c(camera.camera_center());
  if (c.w() < 0.0)
    c.rescale_w(-c.w());

  auto n = normals.begin();
  for (unsigned int i=0; i<tris.size(); ++i, ++n) {
    const vgl_point_3d<double>& v1 = verts3d[tris(i,0)];
    vgl_vector_3d<double> d(c.x()-v1.x()*c.w(),
                            c.y()-v1.y()*c.w(),
                            c.z()-v1.z()*c.w());
    if (dot_product(*n,d) < 0.0)
      continue;
    imesh_render_triangle(verts2d[tris(i,0)],
                          verts2d[tris(i,1)],
                          verts2d[tris(i,2)],
                          image);
  }
}


//: project the mesh onto the image plane using the camera
//  Set each pixel of the image to true if the mesh projects onto it
void imesh_project(const imesh_mesh& mesh,
                   const vpgl_proj_camera<double>& camera,
                   vil_image_view<bool>& image)
{
  assert(mesh.vertices().dim() == 3);
  std::vector<vgl_point_2d<double> > verts2d;
  imesh_project_verts(mesh.vertices<3>(), camera, verts2d);
  imesh_render_faces(mesh, verts2d, image);
}


//: project the mesh onto the image plane using the camera
//  Set each pixel of the image to the depth to the mesh
void imesh_project_depth(const imesh_mesh& mesh,
                         const vpgl_proj_camera<double>& camera,
                         vil_image_view<double>& image)
{
  assert(mesh.vertices().dim() == 3);
  std::vector<vgl_point_2d<double> > verts2d;
  std::vector<double> depths;
  image.fill(std::numeric_limits<double>::infinity());
  imesh_project_verts(mesh.vertices<3>(), camera, verts2d, depths);
  imesh_render_faces_interp(mesh, verts2d, depths, image);
}


//: Compute the bounds of the projection of a set of image points
//  The returned bounds are the intersection of the input bounds
// and the bounding box of the points
void imesh_projection_bounds(const std::vector<vgl_point_2d<double> >& img_pts,
                             vgl_box_2d<unsigned int>& bbox)
{
  assert(!img_pts.empty());

  int i0 = bbox.max_x(), i1 = bbox.min_x(),
      j0 = bbox.max_y(), j1 = bbox.min_y();
  for (auto img_pt : img_pts) {
    double x = img_pt.x(), y = img_pt.y();
    int v = static_cast<int>(std::ceil(x))-1;
    if (v < i0) i0 = v;
    v = static_cast<int>(std::floor(x))+1;
    if (v > i1) i1 = v;

    v = static_cast<int>(std::ceil(y))-1;
    if (v < j0) j0 = v;
    v = static_cast<int>(std::floor(y))+1;
    if (v > j1) j1 = v;
  }
  if (i0 > static_cast<int>(bbox.min_x())) bbox.set_min_x(static_cast<unsigned int>(i0));
  if (i1 < static_cast<int>(bbox.max_x())) bbox.set_max_x(static_cast<unsigned int>(i1));
  if (j0 > static_cast<int>(bbox.min_y())) bbox.set_min_y(static_cast<unsigned int>(j0));
  if (j1 < static_cast<int>(bbox.max_y())) bbox.set_max_y(static_cast<unsigned int>(j1));
}


//: back project an image point onto the mesh using the camera
//  Return true if the ray intersects the mesh
int imesh_project_onto_mesh(const imesh_mesh& mesh,
                            const std::vector<vgl_vector_3d<double> >& normals,
                            const std::vector<vgl_point_2d<double> >& verts2d,
                            const vpgl_perspective_camera<double>& camera,
                            const vgl_point_2d<double>& pt_2d,
                            vgl_point_3d<double>& pt_3d)
{
  int tri_idx = -1;

  vgl_line_3d_2_points<double> ray = camera.backproject(pt_2d);
  vgl_vector_3d<double> d = ray.direction();
  normalize(d);

  vgl_point_3d<double> center = camera.get_camera_center();

  // get mesh faces as triangles
  assert(mesh.faces().regularity() == 3);
  const auto& tris =
      static_cast<const imesh_regular_face_array<3>&>(mesh.faces());

  assert(mesh.vertices().dim() == 3);
  const imesh_vertex_array<3>& verts3d = mesh.vertices<3>();

  auto n = normals.begin();
  double depth = std::numeric_limits<double>::infinity();
  int i = 0;
  for (auto itr = tris.begin(); itr != tris.end(); ++itr, ++n, ++i) {
    if (dot_product(*n,d) > 0.0)
      continue;
    const vgl_point_2d<double>& a = verts2d[(*itr)[0]];
    const vgl_point_2d<double>& b = verts2d[(*itr)[1]];
    const vgl_point_2d<double>& c = verts2d[(*itr)[2]];
    if (vgl_triangle_test_inside<double>(a.x(), a.y(), b.x(), b.y(),
                                        c.x(), c.y(), pt_2d.x(), pt_2d.y())) {
      vgl_plane_3d<double> tri(verts3d[(*itr)[0]],
                               verts3d[(*itr)[1]],
                               verts3d[(*itr)[2]]);
      vgl_point_3d<double> pt3 = vgl_intersection(ray, tri);
      double dist = vgl_distance(pt3,center);
      if (dist < depth) {
        depth = dist;
        pt_3d = pt3;
        tri_idx = i;
      }
    }
  }
  return tri_idx;
}


//: back project image points onto the mesh using the camera
//  Returns a vector of all valid 3d points and indices to corresponding 2d points
void imesh_project_onto_mesh(const imesh_mesh& mesh,
                             const std::vector<vgl_vector_3d<double> >& normals,
                             const vpgl_perspective_camera<double>& camera,
                             const std::vector< vgl_point_2d<double> >& pts_2d,
                             std::vector<unsigned int >& idx_2d,
                             std::vector<vgl_point_3d<double> >& pts_3d)
{
  std::vector<vgl_point_2d<double> > verts2d;
  assert(mesh.vertices().dim() == 3);
  imesh_project_verts(mesh.vertices<3>(), camera, verts2d);

  vgl_point_3d<double> pt_3d;
  for (unsigned int i=0; i<pts_2d.size(); ++i)
  {
    if (imesh_project_onto_mesh(mesh, normals, verts2d, camera, pts_2d[i], pt_3d)) {
      pts_3d.push_back(pt_3d);
      idx_2d.push_back(i);
    }
  }
}


// anonymous namespace
namespace{

void
xy_to_barycentric(const vgl_point_2d<double>& pt_xy,
                  const vgl_point_2d<double>& a,
                  const vgl_point_2d<double>& b,
                  const vgl_point_2d<double>& c,
                  vgl_point_2d<double>& pt_bary)
{
  vgl_vector_2d<double> v1(b-a), v2(c-a), vp(pt_xy-a);
  vgl_vector_2d<double> v1n(-v1.y(), v1.x());
  vgl_vector_2d<double> v2n(v2.y(), -v2.x());
  double s = 1.0/(v2.y()*v1.x() - v2.x()*v1.y());
  pt_bary.y() = s * dot_product(v1n,vp);
  pt_bary.x() = s * dot_product(v2n,vp);
}

void
barycentric_to_xy(const vgl_point_2d<double>& pt_bary,
                  const vgl_point_2d<double>& a,
                  const vgl_point_2d<double>& b,
                  const vgl_point_2d<double>& c,
                  vgl_point_2d<double>& pt_xy)
{
  double z = 1.0 - pt_bary.x() - pt_bary.y();
  pt_xy.x() = z*a.x() + pt_bary.x()*b.x() + pt_bary.y()*c.x();
  pt_xy.y() = z*a.y() + pt_bary.x()*b.y() + pt_bary.y()*c.y();
}
// end of namespace
};


//: back project an image point onto the mesh using the camera
//  The resulting point is in barycentric coordinates for the returned triangle
//  Returns the index of the intersected triangle, or -1 for no intersection
int imesh_project_onto_mesh_barycentric(const imesh_mesh& mesh,
                                        const std::vector<vgl_vector_3d<double> >& normals,
                                        const std::vector<vgl_point_2d<double> >& verts2d,
                                        const vpgl_perspective_camera<double>& camera,
                                        const vgl_point_2d<double>& pt_img,
                                        vgl_point_2d<double>& pt_bary)
{
  vgl_point_3d<double> pt_3d;

  // get mesh faces as triangles
  assert(mesh.faces().regularity() == 3);
  const auto& tris =
      static_cast<const imesh_regular_face_array<3>&>(mesh.faces());

  int tri_idx = imesh_project_onto_mesh(mesh, normals, verts2d, camera, pt_img, pt_3d);
  if (tri_idx >= 0) {
    const imesh_regular_face<3>& tri = tris[tri_idx];
    const vgl_point_2d<double>& a = verts2d[tri[0]];
    const vgl_point_2d<double>& b = verts2d[tri[1]];
    const vgl_point_2d<double>& c = verts2d[tri[2]];
    xy_to_barycentric(pt_img,a,b,c,pt_bary);
  }

  return tri_idx;
}

//: back project an image point onto the mesh using the camera
//  Then project from the mesh into normalized texture coordinate (UV)
//  Assumes the mesh has both normals and texture coordinates
//  \returns the index of the intersected triangle, or -1 for no intersection
int imesh_project_onto_mesh_texture(const imesh_mesh& mesh,
                                    const std::vector<vgl_point_2d<double> >& verts2d,
                                    const vpgl_perspective_camera<double>& camera,
                                    const vgl_point_2d<double>& pt_img,
                                    vgl_point_2d<double>& pt_uv)
{
  if (!mesh.faces().has_normals() || !mesh.has_tex_coords())
    return -1;

  // get mesh faces as triangles
  assert(mesh.faces().regularity() == 3);
  const auto& tris =
      static_cast<const imesh_regular_face_array<3>&>(mesh.faces());

  const std::vector<vgl_point_2d<double> >& tex_coords = mesh.tex_coords();

  vgl_point_2d<double> pt_bary;
  int tri_idx = imesh_project_onto_mesh_barycentric(mesh, tris.normals(),
                                                    verts2d, camera,
                                                    pt_img, pt_bary);
  unsigned int i1, i2, i3;
  if (mesh.has_tex_coords() == imesh_mesh::TEX_COORD_ON_VERT) {
    const imesh_regular_face<3>& tri = tris[tri_idx];
    i1 = tri[0];
    i2 = tri[1];
    i3 = tri[2];
  }
  else {
    imesh_half_edge_set::f_const_iterator fi = mesh.half_edges().face_begin(tri_idx);
    assert(tris[tri_idx][0] == fi->vert_index());
    i1 = fi->edge_index();
    i2 = (++fi)->edge_index();
    i3 = (++fi)->edge_index();
  }
  if (tri_idx >= 0) {
    const vgl_point_2d<double>& a = tex_coords[i1];
    const vgl_point_2d<double>& b = tex_coords[i2];
    const vgl_point_2d<double>& c = tex_coords[i3];
    barycentric_to_xy(pt_bary,a,b,c,pt_uv);
  }

  return tri_idx;
}


//: project a texture point onto a mesh face index with barycentric coordinates
//  \returns the index of the intersected triangle, or -1 for no intersection
int imesh_project_texture_to_barycentric(const imesh_mesh& mesh,
                                         const vgl_point_2d<double>& pt_2d,
                                         vgl_point_2d<double>& pt_uv)
{
  // get mesh faces as triangles
  assert(mesh.faces().regularity() == 3);
  const auto& tris =
      static_cast<const imesh_regular_face_array<3>&>(mesh.faces());

  if (!mesh.has_tex_coords())
    return imesh_invalid_idx;

  const std::vector<vgl_point_2d<double> >& tc = mesh.tex_coords();

  if (mesh.has_tex_coords() == imesh_mesh::TEX_COORD_ON_VERT)
  {
    int i = 0;
    for (auto itr = tris.begin(); itr != tris.end(); ++itr, ++i) {
      const vgl_point_2d<double>& a = tc[(*itr)[0]];
      const vgl_point_2d<double>& b = tc[(*itr)[1]];
      const vgl_point_2d<double>& c = tc[(*itr)[2]];
      double dsc = vgl_triangle_test_discriminant(a.x(),a.y(),b.x(),b.y(),c.x(),c.y());
      if (dsc <= 0.0) continue;
      xy_to_barycentric(pt_2d,a,b,c,pt_uv);
      if (pt_uv.x() < 0.0) continue;
      if (pt_uv.y() < 0.0) continue;
      if (pt_uv.x()+pt_uv.y() >1.0) continue;

      return i;
    }
  }

  return imesh_invalid_idx;
}

#if 1
namespace{

// intersect a straight line in texture space with the texture triangles
bool trace_texture(const imesh_mesh& mesh,
                   const vgl_point_2d<double>& end_xy,
                   std::vector<unsigned long>& idxs,
                   std::vector<vgl_point_2d<double> >& isect_bary)
{
  const std::vector<vgl_point_2d<double> >& tc = mesh.tex_coords();
  assert(mesh.faces().regularity() == 3);
  const auto& tris =
      static_cast<const imesh_regular_face_array<3>&>(mesh.faces());
  assert(mesh.has_half_edges());
  const imesh_half_edge_set& he = mesh.half_edges();
  assert(!isect_bary.empty());

  double eps = 1e-2;
  double eps2 = eps*eps;

  vgl_point_2d<double> nbary;

  // get the last point and start from here
  unsigned long tidx = idxs.back();
  vgl_point_2d<double> bary = isect_bary.back();

  // split into intersection type and half edge index
  unsigned long  heidx = tidx>>2;
  //unsigned char type = tidx & 3;

  for (;;)
  {
    unsigned int fidx = he[heidx].face_index();
    const vgl_point_2d<double>& a = tc[tris[fidx][0]];
    const vgl_point_2d<double>& b = tc[tris[fidx][1]];
    const vgl_point_2d<double>& c = tc[tris[fidx][2]];
    // compute barycentric coordinates of the end point
    xy_to_barycentric(end_xy,a,b,c,nbary);
    // compute the line direction in the current barycentric coordinates
    vgl_vector_2d<double> duv(bary-nbary);
    // if the last point was close enough to the end
    if (duv.sqr_length() <eps2)
      return true;

    // if the end point is inside the triangle then we are done
    if (nbary.x() > 0 && nbary.y() > 0 && 1.0-nbary.x()-nbary.y()>0) {
      idxs.push_back(heidx<<2); // this is a face point
      isect_bary.push_back(nbary);
      return true;
    }
    else if ((nbary.x()==0.0 && nbary.y()>0 && nbary.y()<1) ||
            (nbary.y()==0.0 && nbary.x()>0 && nbary.x()<1) ||
            (nbary.x()+nbary.y()==1.0 && nbary.x()>0 && nbary.y()>0)) {
      idxs.push_back((heidx<<2) + 1); // this is an edge point
      isect_bary.push_back(nbary);
      return true;
    }

    // intersect the line with the with the current triangle
    unsigned char s = imesh_triangle_intersect(bary.x(),bary.y(),duv.x(),duv.y(), eps);
    typedef imesh_half_edge_set::f_const_iterator fitr;
    fitr fi = he.face_begin(fidx);
    assert(fi->face_index() == fidx);
    if (s == 3 || s == 5 || s == 6) // intersects with an edge
    {
      double t = 1 - bary.x();
      if (s==6) {
        ++fi;
        t = bary.x();
      }
      if (s==5) {
        ++fi;
        ++fi;
        t = bary.y();
      }


      // switch to the triangle on the other side of the edge
      heidx = fi->pair_index();
      fidx = he[heidx].face_index();

      // check if the next triangle is out of bounds
      if (fidx == imesh_invalid_idx ||
          (!mesh.valid_tex_faces().empty() &&
           !mesh.valid_tex_faces()[fidx]) )
      {
        // add the boundary point and exit
        idxs.push_back((fi->half_edge_index()<<2) + 1); // this is an edge point
        isect_bary.push_back(bary);
        return false;
      }

      // map the barycentric coordinates into those of the adjacent triangle
      fi = he.face_begin(fidx);
      unsigned char s2 = 0;
      for (; fi->half_edge_index() != heidx && s2<4; ++s2, ++fi)
        ;
      bary.set(0,0);
      switch (s2)
      {
        case 0: bary.x() = t; break;
        case 1: bary.x() = 1-t; bary.y()=t; break;
        case 2: bary.y() = 1-t; break;
        default:
          assert(false);
      }

      // insert the intersection
      idxs.push_back((heidx<<2) + 1); // this is an edge point
      isect_bary.push_back(bary);
    }
    else if (s == 1 || s == 2 || s == 4) // intersects with a vertex
    {
      if (s==2) {
        ++fi;
      }
      if (s==4) {
        ++fi;
        ++fi;
      }
      assert(fi->vert_index() == tris[fidx][(s==4)?2:s-1]);

      // find the direction vector in texture coordinates
      vgl_vector_2d<double> dir(end_xy-tc[fi->vert_index()]);
      normalize(dir);

      // find the face adjacent to the vertex that contains the outgoing vector
      imesh_half_edge_set::v_const_iterator vi(fi);
      imesh_half_edge_set::v_const_iterator vend(vi);
      ++vi;
      double max_ang = -vnl_math::twopi;
      unsigned long max_heidx = 0;
      for (; vi!=vend; ++vi) {
        vgl_vector_2d<double> d(tc[he[vi->pair_index()].vert_index()]
                                - tc[vi->vert_index()]);
        // rotate to compute angle relative to dir
        double ang = std::atan2(d.y()*dir.x()-d.x()*dir.y(),
                               d.x()*dir.x()+d.y()*dir.y());
        if (ang > 0) ang -= vnl_math::twopi;
        if (ang > max_ang) {
          max_ang = ang;
          max_heidx = vi->half_edge_index();
        }
      }
      // switch to the triangle on the other side of the vertex
      heidx = max_heidx;
      fidx = he[heidx].face_index();

      // check if the next triangle is out of bounds
      if (fidx == imesh_invalid_idx ||
          (!mesh.valid_tex_faces().empty() &&
           !mesh.valid_tex_faces()[fidx]) )
      {
        // add the boundary point and exit
        idxs.push_back((fi->half_edge_index()<<2) + 2); // this is an edge point
        isect_bary.push_back(bary);
        return false;
      }

      // map the barycentric coordinates into those of the next triangle
      unsigned vidx = fi->vert_index();
      fi = he.face_begin(fidx);
      unsigned char s2 = 0;
      for (; fi->vert_index() != vidx && s2<4; ++s2, ++fi)
        ;
      bary.set(0,0);
      switch (s2)
      {
        case 0: break;
        case 1: bary.x() = 1; break;
        case 2: bary.y() = 1; break;
        default:
          assert(false);
      }

      // insert the intersection
      idxs.push_back((heidx<<2) + 2); // this is a vertex point
      isect_bary.push_back(bary);
    }
    else
    {
      std::cout << "invalid intersection" << std::endl;
      return false;
    }
  }
  return false; // will never be reached! -- just avoids a compiler warning
}
// end of namespace
}
#endif

//: project a texture polygon into barycentric coordinates
//  \returns true if the polygon is not clipped by the mesh texture
//  \returns the vector of barycentric points by reference
//  \returns a vector of coded half edge indices
//  the half edge indices are scaled by a factor of 4
//  the last two bits indicate the intersection type:
//  - 0 for face
//  - 1 for edge
//  - 2 for vertex
//  barycentric coordinate refer to the adjacent triangle
//  \returns a mapping from each original point into barycentric points.
//  if an original point is not mapped the value is -1
bool imesh_project_texture_to_barycentric(const imesh_mesh& mesh,
                                          const std::vector<vgl_point_2d<double> >& pts_2d,
                                          std::vector<vgl_point_2d<double> >& pts_uv,
                                          std::vector<unsigned long>& idxs,
                                          std::vector<int>& map_back)
{
  bool clipped = false;
  const unsigned int npts = pts_2d.size();
  pts_uv.clear();
  idxs.clear();
  map_back.clear();
  map_back.resize(npts,-1);
  assert(mesh.faces().regularity() == 3);
  assert(mesh.has_half_edges());
  const imesh_half_edge_set& he = mesh.half_edges();

  // find a starting point
  unsigned int i;
  unsigned int idx=0;
  vgl_point_2d<double> pt_uv;
  for (i=0; i<npts; ++i)
  {
    const vgl_point_2d<double>& pt = pts_2d[i];
    if (pt.x() >= 0.0 && pt.x() <= 1.0 && pt.y() >= 0.0 && pt.y() <= 1.0)
    {
      idx = imesh_project_texture_to_barycentric(mesh,pt,pt_uv);
      if (idx != imesh_invalid_idx)
        break;
    }
    clipped = true;
  }
  if (i==npts) // includes the case npts==0; in which case idx would be uninitialized
    return false;

  idxs.push_back(he.face_begin(idx)->half_edge_index()<<2);
  pts_uv.push_back(pt_uv);
  map_back[i] = 0;

  // trace the rest of the polygon
  bool valid = true;
  int num_found = 0;
  for (unsigned int j=(i+1)%npts; j!=i; j=(j+1)%npts, ++num_found )
  {
    valid = trace_texture(mesh,pts_2d[j],idxs,pts_uv);
    if (!valid)
      break; // curve is clipped so exit here
    map_back[j] = idxs.size()-1;
  }
  // if the curve was clipped, try tracing in reverse to get the rest
  if (!valid)
  {
    std::vector<vgl_point_2d<double> > rev_pts_uv(1,pts_uv.front());
    std::vector<unsigned long> rev_idxs(1,idxs.front());
    std::vector<int> rev_map_back(pts_2d.size(),-1);
    rev_map_back[i]=0;
    const unsigned int step = npts-1;
    for (unsigned int j=(i+step)%npts; j!=i; j=(j+step)%npts )
    {
      valid = trace_texture(mesh,pts_2d[j],rev_idxs,rev_pts_uv);
      if (!valid)
        break; // curve is clipped so exit here
      rev_map_back[j] = rev_idxs.size()-1;
    }

    // reverse the reverse searching arrays and append the original search
    for (unsigned int j=0; j<npts; ++j)
    {
      if (map_back[j] >=0)
        map_back[j] += rev_idxs.size()-1;
      else if (rev_map_back[j] >= 0)
        map_back[j] = rev_idxs.size()-1 - rev_map_back[j];
    }

    std::reverse(rev_pts_uv.begin(),rev_pts_uv.end());
    std::reverse(rev_idxs.begin(),rev_idxs.end());
    unsigned int num_new_pts = rev_pts_uv.size();
    rev_pts_uv.resize(rev_pts_uv.size()+pts_uv.size()-1);
    std::copy(pts_uv.begin()+1, pts_uv.end(), rev_pts_uv.begin()+num_new_pts);
    rev_idxs.resize(rev_idxs.size()+idxs.size()-1);
    std::copy(idxs.begin()+1, idxs.end(), rev_idxs.begin()+num_new_pts);


    idxs.swap(rev_idxs);
    pts_uv.swap(rev_pts_uv);

    return true; // return true to indicate clipping
  }

  // connect back to the first point, but remove the duplicated first point
  valid = trace_texture(mesh,pts_2d[i],idxs,pts_uv);
  if (!valid)
    return true;

  idxs.pop_back();
  pts_uv.pop_back();

  assert(pts_uv.size() == idxs.size());

  return clipped;
}


//: compute the matrix that maps texture points to 3-d for a given triangle index
// $(u,v,1)$ maps into 3-d $(x,y,z)$
vnl_matrix_fixed<double,3,3>
imesh_project_texture_to_3d_map(const imesh_mesh& mesh, unsigned int tidx)
{
  assert(mesh.has_tex_coords());
  const auto& triangles =
      static_cast<const imesh_regular_face_array<3>&>(mesh.faces());
  const imesh_regular_face<3>& tri = triangles[tidx];
  const std::vector<vgl_point_2d<double> >& tex = mesh.tex_coords();
  const imesh_vertex_array<3>& verts = mesh.vertices<3>();

  vnl_matrix_fixed<double,3,3> M1;
  {
    const vgl_point_2d<double>& a = tex[tri[0]];
    const vgl_point_2d<double>& b = tex[tri[1]];
    const vgl_point_2d<double>& c = tex[tri[2]];
    vgl_vector_2d<double> v1(b-a), v2(c-a);
    double s = 1.0 / (v2.y()*v1.x() - v2.x()*v1.y());
    M1(0,0) = v2.y()*s;   M1(0,1) = -v2.x()*s;   M1(0,2) = -a.x()*s;
    M1(1,0) = -v1.y()*s;  M1(1,1) = v1.x()*s;    M1(1,2) = -a.y()*s;
    M1(2,0) = 0.0;        M1(2,1) = 0.0;         M1(2,2) = 1.0;
  }

  vnl_matrix_fixed<double,3,3> M2;
  {
    const imesh_vertex<3>& a = verts[tri[0]];
    const imesh_vertex<3>& b = verts[tri[1]];
    const imesh_vertex<3>& c = verts[tri[2]];
    vgl_vector_3d<double> v1(b[0]-a[0],b[1]-a[1],b[2]-a[2]),
                          v2(c[0]-a[0],c[1]-a[1],c[2]-a[2]);
    M2(0,0) = v1.x();   M2(0,1) = v2.x();   M2(0,2) = a[0];
    M2(1,0) = v1.y();   M2(1,1) = v2.y();   M2(1,2) = a[1];
    M2(2,0) = v1.z();   M2(2,1) = v2.z();   M2(2,2) = a[2];
  }


  return M2*M1;
}


//: compute the affine matrix that maps triangle (a1,b1,c1) to (a2,b2,c2)
vnl_matrix_fixed<double,3,3>
imesh_affine_map(const vgl_point_2d<double>& a1,
                 const vgl_point_2d<double>& b1,
                 const vgl_point_2d<double>& c1,
                 const vgl_point_2d<double>& a2,
                 const vgl_point_2d<double>& b2,
                 const vgl_point_2d<double>& c2)
{
  // triangle 1 to barycentric coordinates
  vnl_matrix_fixed<double,3,3> M1;
  {
    vgl_vector_2d<double> v1(b1-a1), v2(c1-a1);
    double s = 1.0 / (v2.y()*v1.x() - v2.x()*v1.y());
    M1(0,0) = v2.y()*s;   M1(0,1) = -v2.x()*s;   M1(0,2) = (a1.y()*c1.x()-a1.x()*c1.y())*s;
    M1(1,0) = -v1.y()*s;  M1(1,1) = v1.x()*s;    M1(1,2) = (a1.x()*b1.y()-a1.y()*b1.x())*s;
    M1(2,0) = 0.0;        M1(2,1) = 0.0;         M1(2,2) = 1.0;
  }

  // barycentric coordinates to triangle 2
  vnl_matrix_fixed<double,3,3> M2;
  {
    vgl_vector_2d<double> v1(b2-a2), v2(c2-a2);
    M2(0,0) = v1.x();   M2(0,1) = v2.x();   M2(0,2) = a2.x();
    M2(1,0) = v1.y();   M2(1,1) = v2.y();   M2(1,2) = a2.y();
    M2(2,0) = 0.0;      M2(2,1) = 0.0;      M2(2,2) = 1.0;
  }

  return M2*M1;
}


//: project barycentric coordinates with an index to texture space
//  \param idx is the face index
vgl_point_2d<double>
imesh_project_barycentric_to_texture(const imesh_mesh& mesh,
                                     const vgl_point_2d<double>& pt_uv,
                                     unsigned int idx)
{
  assert(mesh.has_tex_coords());
  const std::vector<vgl_point_2d<double> >& tc = mesh.tex_coords();
  const vgl_point_2d<double>& a = tc[mesh.faces()(idx,0)];
  const vgl_point_2d<double>& b = tc[mesh.faces()(idx,1)];
  const vgl_point_2d<double>& c = tc[mesh.faces()(idx,2)];
  vgl_point_2d<double> p;
  barycentric_to_xy(pt_uv,a,b,c,p);
  return p;
}


//: project barycentric coordinates with an index the mesh surface (3D)
//  \param idx is the face index
vgl_point_3d<double>
imesh_project_barycentric_to_mesh(const imesh_mesh& mesh,
                                  const vgl_point_2d<double>& pt_uv,
                                  unsigned int idx)
{
  vgl_point_3d<double> pt(0,0,0);
  unsigned int v1 = mesh.faces()(idx,0);
  unsigned int v2 = mesh.faces()(idx,1);
  unsigned int v3 = mesh.faces()(idx,2);
  const imesh_vertex_array<3>& verts = mesh.vertices<3>();
  pt += (1-pt_uv.x()-pt_uv.y())*vgl_vector_3d<double>(verts[v1][0],verts[v1][1],verts[v1][2]);
  pt += pt_uv.x()*vgl_vector_3d<double>(verts[v2][0],verts[v2][1],verts[v2][2]);
  pt += pt_uv.y()*vgl_vector_3d<double>(verts[v3][0],verts[v3][1],verts[v3][2]);

  return pt;
}
