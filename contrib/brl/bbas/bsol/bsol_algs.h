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
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <bsol/dll.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
class bsol_algs
{
 public:
  ~bsol_algs();

  //: Finds a bounding box for a set of vsol_line_2ds
  static vbl_bounding_box<double,2> 
    bounding_box(vcl_vector<vsol_line_2d_sptr> const& lines);

  //:do two boxes intersect?
  static bool meet(vsol_box_2d_sptr const & a, vsol_box_2d_sptr const & b);


  //:find the intersection of two boxes. Return false if no intersection
  static bool intersection(vsol_box_2d_sptr const & a,
                           vsol_box_2d_sptr const & b,
                           vsol_box_2d_sptr& a_int_b);

  //:construct a vsol_polygon from a vsol_box
  static vsol_polygon_2d_sptr poly_from_box(vsol_box_2d_sptr const& box);

  //: Transform a vsol_polygon_2d with a general homography
  static bool homography(vsol_polygon_2d_sptr const& p,
                  vgl_h_matrix_2d<double> const& H,
                  vsol_polygon_2d_sptr& Hp);
  

 private:
  bsol_algs();
};

#endif
