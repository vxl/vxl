#ifndef bwm_delaunay_tri_h_
#define bwm_delaunay_tri_h_
//:
// \file
// \brief Delaunay triangulation for a set of 3D points.
//   Triangulation happens in 2D by only using (x,y) component and moving them back to 3D
//
// \author Vishal Jain
// \date   June 10, 2008
//
// \verbatim
//  Modifications
//   Jun 10, 2008  Gamze Tunali
//                 Created a class and converted all the variable to vxl types
//                 and added the sorting as a part of the code.
// \endverbatim

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>

class bwm_delaunay_tri
{
 public:
  bwm_delaunay_tri() {}
  ~bwm_delaunay_tri() {}

  int triangulate(std::vector<vgl_point_3d<double> >& pxyz,
                  std::vector<vgl_point_3d<int> >& v,
                  int &ntri);
 private:

  static double EPSILON;

  bool CircumCircle(vgl_point_2d<double> p,
                    vgl_point_2d<double> p1,
                    vgl_point_2d<double> p2,
                    vgl_point_2d<double> p3,
                    vgl_point_2d<double> &c, double &r);

  static int XYZCompare(const void *v1, const void *v2);
};

#endif
