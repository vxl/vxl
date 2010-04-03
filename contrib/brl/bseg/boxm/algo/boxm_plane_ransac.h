#ifndef boxm_plane_ransac_h_
#define boxm_plane_ransac_h_

#include <vgl/vgl_infinite_line_3d.h>
#include <vgl/vgl_intersection.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_vector_3d.h>

#include <vcl_cstdlib.h> // for rand()
#include <vcl_iostream.h>

template <class T>
void boxm_plane_ransac(vcl_vector<vgl_plane_3d<T> > const& planes,
                       vcl_vector<unsigned>& indices,
                       int threshold)
{
  int num_planes = planes.size();

  while (indices.size() < threshold) {
    indices.resize(0);
    // select two planes randomly
    int index1 = vcl_rand() % num_planes;
    int index2 = vcl_rand() % num_planes;
    vgl_plane_3d<T> plane1 = planes[index1];
    vgl_plane_3d<T> plane2 = planes[index2];

    // intersect them to get a line
    vgl_infinite_line_3d<T> line;
    if (vgl_intersection(plane1, plane2, line)) {
      vgl_vector_3d<T> line_dir = line.direction();

      // test the line on each plane
      for (unsigned i=0; i<num_planes; i++) {
        vgl_plane_3d<T> plane = planes[i];
        vgl_vector_3d<T> normal = plane.normal();
        // see if the line direction and plane normal is perpendicular
        T res = dot_product(normal,line_dir);
        if (res < 0.0001)  {
          vgl_point_3d<T> p=line.point();
          if (plane.contains(p,0.0001)) {
            indices.push_back(i);
          }
        }
      }
    }
  }
  vcl_cerr << "The number of fit planes=" << indices.size() << " out of " << planes.size() << vcl_endl;
}

#endif
