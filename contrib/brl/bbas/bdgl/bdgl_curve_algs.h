#ifndef bdgl_curve_algs_h_
#define bdgl_curve_algs_h_

//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief vdgl_digital_curve algorithms 
//
// \verbatim
// Initial version November 25, 2002
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <btol/dll.h>
#include <vgl/vgl_line_2d.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <vdgl/vdgl_digital_curve_sptr.h>

class bdgl_curve_algs
{
 public:
  static BTOL_DLL_DATA const double tol;
  ~bdgl_curve_algs();

  //:Finds the edgel index on a digital curve closest to the given point
  static int closest_point(vdgl_edgel_chain_sptr& ec,
                           const double x, const double y);

  //:Finds the x-y coordinates on a digital curve closest to the given point
  static bool closest_point(vdgl_digital_curve_sptr& dc,
                           const double x, const double y,
                            double& xc, double& yc);

  //:Reverses the edgel chain
  static vdgl_digital_curve_sptr reverse(vdgl_digital_curve_sptr& dc);

  //:Intersect the curve with an infinite line
  static bool intersect_line(vdgl_digital_curve_sptr& dc,
                             vgl_line_2d<double>& line,
                             vcl_vector<vgl_point_2d<double> >& pts);
 private:
  bdgl_curve_algs();
};

#endif
