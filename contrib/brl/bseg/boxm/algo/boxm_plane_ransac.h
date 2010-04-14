#ifndef boxm_plane_ransac_h_
#define boxm_plane_ransac_h_

#include <vgl/vgl_infinite_line_3d.h>
#include <vgl/vgl_intersection.h>
#include <vgl/algo/vgl_intersection.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_vector_3d.h>

#include <vcl_cstdlib.h> // for rand()
#include <vcl_iostream.h>
#include <vcl_list.h>
#define ITER_MAX 100

template <class T>
bool boxm_plane_ransac(vcl_vector<vgl_plane_3d<T> > const& planes,
                       vcl_vector<T> weights,
                       vgl_infinite_line_3d<T>& line,
                       T &residual,
                       unsigned int threshold)
{
  unsigned int num_planes = planes.size();
  vcl_list<vgl_plane_3d<T> > fit_planes;
  T min_res=1e10;
  line=vgl_infinite_line_3d<T>(vgl_vector_2d<T>(0,0),vgl_vector_3d<T>(1,1,1));
  vcl_vector<T> ws;
  bool set=false;
  for (unsigned iter=0; iter<ITER_MAX; iter++) {
    fit_planes.resize(0);
    ws.resize(0);
    // select two planes randomly
    int index1 = vcl_rand() % num_planes;
    int index2 = vcl_rand() % num_planes;
    vgl_plane_3d<T> plane1 = planes[index1];
    vgl_plane_3d<T> plane2 = planes[index2];

    // intersect them to get a line
    vgl_infinite_line_3d<T> inters_line;
    if (vgl_intersection(plane1, plane2, inters_line)) {
      vgl_vector_3d<T> line_dir = inters_line.direction();

      // test the line on each plane
      for (unsigned i=0; i<num_planes; ++i) {
        vgl_plane_3d<T> plane = planes[i];
        vgl_vector_3d<T> normal = plane.normal();
        // see if the line direction and plane normal is perpendicular
        T res = dot_product(normal,line_dir);
        if (res < 0.0001)  {
          vgl_point_3d<T> p=inters_line.point();
          if (plane.contains(p,0.0001f)) {
            fit_planes.push_back(plane);
            ws.push_back(weights[i]);
          }
        }
      }
    }
    // intersect the selected planes
    if (fit_planes.size() > threshold) {
      float res=0;
      vgl_infinite_line_3d<T> l;
      bool good = vgl_intersection(fit_planes, ws, l, res);
      if (good) {
        if (res<min_res) {
          min_res=res;
          line=l;
          set=true;
        }
      }
    }
  }
  residual=min_res;
  return set;
}

#endif
