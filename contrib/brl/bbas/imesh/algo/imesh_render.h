// This is brl/bbas/imesh/algo/imesh_render.h
#ifndef imesh_render_h_
#define imesh_render_h_

//:
// \file
// \brief Functions to render the faces of a mesh with projected coordinates
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 6/2/08
//
// \verbatim
//  Modifications
// \endverbatim

#include <imesh/imesh_mesh.h>
#include <vil/vil_image_view.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_triangle_scan_iterator.h>





//: Render a triangle defined by its vertices
// for each 3d point, X and Y map to image coordinates and Z maps to depth
// The values i1,i2,i3 are interpolated and rendered into image
template <class T>
void imesh_render_triangle_label(const vgl_point_3d<double>& v1,
                                 const vgl_point_3d<double>& v2,
                                 const vgl_point_3d<double>& v3,
                                 const T& label,
                                       vil_image_view<T>& image,
                                       vil_image_view<double>& depth_img)
{
  assert(depth_img.ni() == image.ni());
  assert(depth_img.nj() == image.nj());

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
  for(tsi.reset(); tsi.next(); ){
    int y = tsi.scany();
    if(y<0 || y>=int(image.nj())) continue;
    int min_x = tsi.startx();
    int max_x = tsi.endx();
    if(min_x >= (int)image.ni() || max_x < 0)
      continue;
    if(min_x < 0) min_x = 0;
    if(max_x >= (int)image.ni()) max_x = image.ni()-1;
    double new_i = B*y+C;
    for (int x = min_x; x <= max_x; ++x){
      double depth = new_i + A*x;
      if(depth < depth_img(x,y)){
        depth_img(x,y) = depth;
        image(x,y) = label;
      }
    }
  }
}






#endif // imesh_render_h_
