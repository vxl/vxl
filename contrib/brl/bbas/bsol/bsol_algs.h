#ifndef bsol_algs_h_
#define bsol_algs_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief Miscellaneous vsol algorithms
// This is essentially a namespace for static methods;
// there is no public constructor
//
// \verbatim
//  Modifications
//   Initial version May 15, 2002
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vbl/vbl_bounding_box.h>
#include <vgl/vgl_polygon.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_3d_sptr.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <vsol/vsol_box_3d_sptr.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <bsol/dll.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
class bsol_algs
{
 public:
  ~bsol_algs();

  //: Finds a bounding box for a set of vsol_point_2ds
  static vbl_bounding_box<double,2> 
    bounding_box(vcl_vector<vsol_point_2d_sptr> const& points);

  //: Finds a bounding box for a set of vsol_line_2ds
  static vbl_bounding_box<double,2> 
    bounding_box(vcl_vector<vsol_line_2d_sptr> const& lines);

  //: Finds a bounding box for a set of vsol_point_3ds
  static vbl_bounding_box<double,3> 
    bounding_box(vcl_vector<vsol_point_3d_sptr> const& points);


  //: Is a point inside the box
  static bool in(vsol_box_2d_sptr const & a, const double x, const double y);

  //:do two boxes intersect?
  static bool meet(vsol_box_2d_sptr const & a, vsol_box_2d_sptr const & b);


  //:find the intersection of two boxes. Return false if no intersection
  static bool intersection(vsol_box_2d_sptr const & a,
                           vsol_box_2d_sptr const & b,
                           vsol_box_2d_sptr& a_int_b);

  //:find the union of two boxes;
  static bool box_union(vsol_box_2d_sptr const & a,
                        vsol_box_2d_sptr const & b,
                        vsol_box_2d_sptr& a_union_b);

  //:find the convex hull of a set of polygons
  static bool hull_of_poly_set(vcl_vector<vsol_polygon_2d_sptr> const& polys,
                               vsol_polygon_2d_sptr& hull);
  //: Is a point inside the box
  static bool in(vsol_box_3d_sptr const & a, const double x, const double y,
                 const double z);

  //:construct a vsol_polygon from a vgl_polygon
  static vsol_polygon_2d_sptr poly_from_vgl(vgl_polygon<double> const& poly);


  //:construct a vsol_polygon from a vsol_box
  static vsol_polygon_2d_sptr poly_from_box(vsol_box_2d_sptr const& box);

  //:find the closest point in a set
  static vsol_point_2d_sptr
    closest_point(vsol_point_2d_sptr const& p,
                  vcl_vector<vsol_point_2d_sptr> const& point_set,
                  double& d);

  static vsol_point_3d_sptr
    closest_point(vsol_point_3d_sptr const& p,
                  vcl_vector<vsol_point_3d_sptr> const& point_set,
                  double& d);

  //: Transform a vsol_polygon_2d with a general homography
  static bool homography(vsol_polygon_2d_sptr const& p,
                  vgl_h_matrix_2d<double> const& H,
                  vsol_polygon_2d_sptr& Hp);
  
  //: Homographically map a vsol_box_2d, returning the resulting bounding box
  static bool homography(vsol_box_2d_sptr const& b,
                         vgl_h_matrix_2d<double> const& H,
                         vsol_box_2d_sptr& Hb);
  //: print methods
  static void print(vsol_box_2d_sptr const& b);
  static void print(vsol_box_3d_sptr const& b);
  static void print(vsol_point_2d_sptr const& p);
  static void print(vsol_point_3d_sptr const& p);
 private:
  bsol_algs();
};

#endif
