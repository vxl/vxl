// This is brl/bbas/imesh/algo/imesh_render.cxx
#include <iostream>
#include <limits>
#include "imesh_render.h"
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <imesh/algo/imesh_project.h>
#include <imesh/imesh_operations.h>
#include <vil/vil_bilin_interp.h>

//: Render a textured triangle defined by its vertices
// \p v1,v2,v3 are coordinates in the projected image (plus depth)
// \p t1,t2,t3 are corresponding texture coordinates (in the unit square)
void imesh_render_triangle_texture(const vgl_point_3d<double>& v1,
                                   const vgl_point_3d<double>& v2,
                                   const vgl_point_3d<double>& v3,
                                   const vgl_point_2d<double>& t1,
                                   const vgl_point_2d<double>& t2,
                                   const vgl_point_2d<double>& t3,
                                   const vil_image_view<vxl_byte>& texture,
                                   vil_image_view<vxl_byte>& image,
                                   vil_image_view<double>& depth_img)
{
  assert(depth_img.ni() == image.ni());
  assert(depth_img.nj() == image.nj());

  unsigned tw = texture.ni();
  unsigned th = texture.nj();
  vnl_matrix_fixed<double,3,3> texmap =
      imesh_affine_map(vgl_point_2d<double>(v1.x(),v1.y()),
                       vgl_point_2d<double>(v2.x(),v2.y()),
                       vgl_point_2d<double>(v3.x(),v3.y()),
                       vgl_point_2d<double>(t1.x()*tw,(1-t1.y())*th),
                       vgl_point_2d<double>(t2.x()*tw,(1-t2.y())*th),
                       vgl_point_2d<double>(t3.x()*tw,(1-t3.y())*th));

  vgl_triangle_scan_iterator<double> tsi;
  tsi.a.x = v1.x();  tsi.a.y = v1.y();
  tsi.b.x = v2.x();  tsi.b.y = v2.y();
  tsi.c.x = v3.x();  tsi.c.y = v3.y();
  vgl_vector_3d<double> b1(v2.x()-v1.x(), v2.y()-v1.y(), v2.z()-v1.z());
  vgl_vector_3d<double> b2(v3.x()-v1.x(), v3.y()-v1.y(), v3.z()-v1.z());
  vgl_vector_3d<double> n = cross_product(b1,b2);
  double A = -n.x()/n.z();
  double B = -n.y()/n.z();
  double C = (v1.x()*n.x() + v1.y()*n.y() + v1.z()*n.z())/n.z();
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
      if (depth < depth_img(x,y)) {
        depth_img(x,y) = depth;
        double tx = texmap[0][0]*x + texmap[0][1]*y + texmap[0][2];
        double ty = texmap[1][0]*x + texmap[1][1]*y + texmap[1][2];
        for (unsigned p=0; p<texture.nplanes(); ++p) {
          image(x,y,p) = (vxl_byte)vil_bilin_interp_safe(texture,tx,ty,p);
        }
      }
    }
  }
}


//: Render the mesh using the camera and a texture image
//  A depth map is also computed and used for occlusion.
//  Texture mapping uses interpolates from the texture image with no
//  additional lighting calculations.
void imesh_render_textured(const imesh_mesh& mesh,
                           const vpgl_proj_camera<double>& camera,
                           const vil_image_view<vxl_byte>& texture,
                           vil_image_view<vxl_byte>& image,
                           vil_image_view<double>& depth_img)
{
  assert(mesh.vertices().dim() == 3);
  assert(mesh.has_tex_coords() == imesh_mesh::TEX_COORD_ON_VERT);
  const std::vector<vgl_point_2d<double> >& tex_coords = mesh.tex_coords();

  std::vector<vgl_point_2d<double> > verts2d;
  std::vector<double> depths;
  depth_img.fill(std::numeric_limits<double>::infinity());
  imesh_project_verts(mesh.vertices<3>(), camera, verts2d, depths);
  assert(tex_coords.size() == verts2d.size());

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

  for (auto tri : *tris) {
    const vgl_point_2d<double>& v1 = verts2d[tri[0]];
    const vgl_point_2d<double>& v2 = verts2d[tri[1]];
    const vgl_point_2d<double>& v3 = verts2d[tri[2]];
    const vgl_point_2d<double>& t1 = tex_coords[tri[0]];
    const vgl_point_2d<double>& t2 = tex_coords[tri[1]];
    const vgl_point_2d<double>& t3 = tex_coords[tri[2]];

    vgl_point_3d<double> p1(v1.x(),v1.y(),depths[tri[0]]);
    vgl_point_3d<double> p2(v2.x(),v2.y(),depths[tri[1]]);
    vgl_point_3d<double> p3(v3.x(),v3.y(),depths[tri[2]]);
    imesh_render_triangle_texture(p1,p2,p3,t1,t2,t3,texture,image,depth_img);
  }
}
