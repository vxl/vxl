//:
// \file
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/vnl_double_3.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_interpolator_linear.h>
#include <vdgl/vdgl_digital_curve.h>
#include <bdgl/bdgl_curve_algs.h>
const double bdgl_curve_algs::tol = 1e-16;

//: Destructor
bdgl_curve_algs::~bdgl_curve_algs()
{
}
//:
//-----------------------------------------------------------------------------
// Finds the index on an edgel_chain closest to the given
// point (x, y). Later this routine can become a method on
// vdgl_edgel_chain.
//-----------------------------------------------------------------------------
int bdgl_curve_algs::closest_point(vdgl_edgel_chain_sptr& ec,
                                   const double x, const double y)

{
  if (!ec)
    {
      vcl_cout<<"In bdgl_curve_algs::closest_point(..) - warning, null chain\n";
      return 0;
    }
  //for now just scan the curve and save the closest point
  double mind = vnl_numeric_traits<double>::maxval;
  int N =ec->size(), imin = 0;

  for (int i = 0; i<N; i++)
    {
      vdgl_edgel ed = ec->edgel(i);
      double d = vcl_sqrt((ed.x()-x)*(ed.x()-x) + (ed.y()-y)*(ed.y()-y));
      if (d<mind)
        {
          mind = d;
          imin = i;
        }
   }
  return imin;
}
//:
//-----------------------------------------------------------------------------
// Finds the closest point, (xc, yc) on a digital curve to a given 
// location (x,y).
// Current implementation is not the best since it is discrete with
// the edgel_chain index.  Ultimately it should use the interpolator
// to refine the location on the digital_curve.
//-----------------------------------------------------------------------------
bool bdgl_curve_algs::closest_point(vdgl_digital_curve_sptr& dc,
                                    const double x, const double y,
                                    double& xc, double& yc)


{
  if (!dc)
    {
      vcl_cout<<"In bdgl_curve_algs::closest_point(..) - warning, null curve\n";
      return false;
    }
  vdgl_interpolator_sptr interp = dc->get_interpolator();
  vdgl_edgel_chain_sptr ec = interp->get_edgel_chain();
  int index = bdgl_curve_algs::closest_point(ec, x, y);
  xc = (*ec)[index].x();
  yc = (*ec)[index].y();
  return true;
}
//:
// It is sometimes necessary to reverse the order of the digital curve
// so that the initial point corresponds to v1 of a topology edge
vdgl_digital_curve_sptr bdgl_curve_algs::reverse(vdgl_digital_curve_sptr& dc)
{
  if (!dc)
    return 0;
  vdgl_interpolator_sptr intrp = dc->get_interpolator();
  vdgl_edgel_chain_sptr ec = intrp->get_edgel_chain();
  int N = ec->size();
  vdgl_edgel_chain_sptr rev_ec = new vdgl_edgel_chain();
  for (int i = 0; i<N; i++)
    rev_ec->add_edgel((*ec)[N-1-i]);
  vdgl_interpolator_sptr rev_intrp = new vdgl_interpolator_linear(rev_ec);
  vdgl_digital_curve_sptr rev_dc = new vdgl_digital_curve(rev_intrp);
  return rev_dc;
}

//: Intersect an infinite line with a line formed by the two input points , p0 and p1.
// It is assumed that the two lines do intersect.
// If they are parallel, "false" is returned.
static bool intersect_crossing(vnl_double_3& line_coefs,
                               vnl_double_3& p0,
                               vnl_double_3& p1,
                               vnl_double_3& inter)
{
  //Form the line from p0 and p1
  vnl_double_3 lv01 = cross_3d(p0, p1);

  //Find the intersection point
  inter = cross_3d(lv01, line_coefs);
  //Check sanity of the intersection
  return vcl_fabs(inter.z()) >= bdgl_curve_algs::tol;
}

//-------------------------------------------------------------
//: intersect an infinite line with the digital curve.
//  If there is no intersection return false. Note that the line can intersect
//  multiple times. All the intersections are returned.
bool bdgl_curve_algs::intersect_line(vdgl_digital_curve_sptr& dc,
                                     vgl_line_2d<double>& line,
                                     vcl_vector<vgl_point_2d<double> >& pts)
{
  if (!dc)
    {
      vcl_cout << "In bdgl_curve_algs::intersect_line - null curve\n";
      return false;
    }
  //compute the resolution of the intersection. The digital curve is
  //typically embedded in image coordinates so we would want to compute the
  //intersection to 0.1 pixels.  The parametrization of the curve is [0,1]
  //so the search interval on the parmeter should be dt = 1/(10*dc->length())
  //That is, this change of dt corresponds to 0.1 pixel on the curve.

  bool intersection = false;
  vnl_double_3 lv(line.a(), line.b(), line.c());

  //We take advantage of the fact that the algebraic distance to
  //a line changes sign if we cross it.
  double t=0, dt = 1/(10*dc->length());
  vnl_double_3 p0(dc->get_x(t), dc->get_y(t), 1.0), p1;
  for (double t=dt; t<=1.0; t+=dt)
  {
    p1.x()=dc->get_x(t); p1.y()=dc->get_y(t); p1.z()= 1.0;
    double sign0 = dot_product(p0, lv);
    double sign1 = dot_product(p1, lv);
    if (vcl_fabs(sign0)<bdgl_curve_algs::tol||              //we have crossed or
        vcl_fabs(sign1)<bdgl_curve_algs::tol||sign0*sign1<=0) // are on the line
      {
        vnl_double_3 inter;
        if (intersect_crossing(lv, p0, p1, inter))
          {
            vgl_point_2d<double> p(inter.x()/inter.z(), inter.y()/inter.z());
            pts.push_back(p);
            intersection = true;
          }
      }
    p0=p1;
  }
  return intersection;
}
