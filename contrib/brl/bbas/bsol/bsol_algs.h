#ifndef bsol_algs_h_
#define bsol_algs_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief Miscelaneous vsol algorithms
//
// \verbatim
// Initial version May 15, 2002
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vbl/vbl_bounding_box.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <bsol/dll.h>
class bsol_algs
{
 public:
  ~bsol_algs();

  //:Finds a bounding box for a set of vsol_line_2ds
  static vbl_bounding_box<double,2> 
    bounding_box(vcl_vector<vsol_line_2d_sptr> const & lines);

 private:
  bsol_algs();
};

#endif
