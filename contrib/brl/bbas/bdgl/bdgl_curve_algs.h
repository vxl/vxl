#ifndef bdgl_curve_algs_h_
#define bdgl_curve_algs_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief vdgl_digital_curve algorithms
//
// \verbatim
//  Modifications
//   Initial version November 25, 2002
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <btol/dll.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <vdgl/vdgl_digital_curve_sptr.h>

class bdgl_curve_algs
{
 public:
  static BTOL_DLL_DATA const double tol;
  static BTOL_DLL_DATA const double synthetic;
  ~bdgl_curve_algs();

  //:Finds the index on a digital curve closest to the given point
  static int closest_point(vdgl_edgel_chain_sptr const& ec,
                           const double x, const double y);

  //:Finds the index on a digital curve closest to the given point
  static double closest_point(vdgl_digital_curve_sptr const& dc,
                              const double x, const double y);

  //:Finds the x-y coordinates on a digital curve closest to the given point
  static bool closest_point(vdgl_digital_curve_sptr const& dc,
                            const double x, const double y,
                            double& xc, double& yc);

  //:Finds location on a digital curve closest to the given point at index
  static  bool closest_point_near(vdgl_edgel_chain_sptr const& ec,
                                  const int index,
                                  const double x, const double y,
                                  double & xc, double & yc);

  //: Finds the end of the edgel chain closest to the given point
  static int closest_end(vdgl_edgel_chain_sptr const & ec,
                         const double x, const double y);

  //:Reverses the edgel chain
  static vdgl_digital_curve_sptr reverse(vdgl_digital_curve_sptr const& dc);

  //:Intersect the curve with an infinite line, return the index values
  static bool intersect_line(vdgl_digital_curve_sptr const& dc,
                             vgl_line_2d<double> & line,
                             vcl_vector<double>& indices);

  //:Intersect the curve with an infinite line, return the points
  static bool intersect_line(vdgl_digital_curve_sptr const& dc,
                             vgl_line_2d<double> & line,
                             vcl_vector<vgl_point_2d<double> >& pts);

  //:Intersect a curve with a line and match the reference edgel point
  static bool match_intersection(vdgl_digital_curve_sptr const& dc,
                                 vgl_line_2d<double>& line,
                                 vgl_point_2d<double> const& ref_point,
                                 double ref_gradient_angle,
                                 vgl_point_2d<double>& point);


  //:Generate points along a digital straight line
  static  bool line_gen(float xs, float ys, float xe, float ye,
                        bool& init, bool& done,
                        float& x, float& y);

  //:Add edgels to an edgel chain along a digital straight line
  static int add_straight_edgels(vdgl_edgel_chain_sptr const& ec,
                                 const double x, const double y,
                                 bool debug = false);

  static void
    smooth_curve(vcl_vector<vgl_point_2d<double> >& curve,double sigma=1.0);

  static vdgl_digital_curve_sptr
    create_digital_curves(vcl_vector<vgl_point_2d<double> > & curve);

 private:
  bdgl_curve_algs();
};

#endif
