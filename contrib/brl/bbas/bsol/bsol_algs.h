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
#include <bsol/dll.h>

class bsol_algs
{
 public:
  ~bsol_algs();

  //: Finds a bounding box for a set of vsol_line_2ds
  static vbl_bounding_box<double,2> 
    bounding_box(vcl_vector<vsol_line_2d_sptr> const& lines);
  //:do two boxes intersect?
  static bool meet(vsol_box_2d_sptr const & a, vsol_box_2d_sptr const & b);
 private:
  bsol_algs();
};

#endif
