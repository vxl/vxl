#ifndef bdgl_curve_algs_h_
#define bdgl_curve_algs_h_

//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief topology algorithms involving edges or edge-centric routines
//
// \verbatim
// Initial version November 25, 2002
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <btol/dll.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <vdgl/vdgl_digital_curve_sptr.h>

class bdgl_curve_algs
{
 public:
  static BTOL_DLL_DATA const double tol;
  ~bdgl_curve_algs();

  //:Finds the edgel index on a digital curve closest to the given point
  static int closest_point(vdgl_edgel_chain_sptr& dc,
                           const double x, const double y);

  //:Reverses the edgel chain
  static vdgl_digital_curve_sptr reverse(vdgl_digital_curve_sptr& dc);

 private:
  bdgl_curve_algs();
};

#endif
