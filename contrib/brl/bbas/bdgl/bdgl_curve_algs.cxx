//:
// \file
#include <vcl_cmath.h>
#include <vcl_cstdlib.h> // for std::abs(int)
#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_3.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_interpolator_linear.h>
#include <vdgl/vdgl_digital_curve.h>
#include <bdgl/bdgl_curve_algs.h>

const double bdgl_curve_algs::tol = 1e-16;
const double bdgl_curve_algs::synthetic = 0;//Indicates synthetic edgel
                                            //default constructor is -1

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
int bdgl_curve_algs::closest_point(vdgl_edgel_chain_sptr const& ec,
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
bool bdgl_curve_algs::closest_point(vdgl_digital_curve_sptr const& dc,
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

//:Interpolates between p0 and p1 finding the closest point to p.
// Returns the parameter t on [0, 1] -> [p0, p1].
static double interpolate_segment(vnl_double_2& p0,
                                  vnl_double_2& p1,
                                   vnl_double_2& p,
                                   vnl_double_2& pc)
{
  double Dx = p1.x()-p0.x(), Dy = p1.y()-p0.y();
  double dx = p.x()-p0.x(), dy = p.y()-p0.y();
  double den = Dx*Dx + Dy*Dy;
  if (den<bdgl_curve_algs::tol)
  {
    pc = p0;
    return 0;
  }
  double t = (dx*Dx + Dy*dy)/den;
  //clip t to lie within the interval
  if (t<0)
    t=0.0;
  if (t>1.0)
    t=1.0;
  pc.x() = t*Dx + p0.x();   pc.y() = t*Dy + p0.y();
  return t;
}

//:
//-----------------------------------------------------------------------------
// Finds the closest point on an edgel chain near a given point (x, y)
// in the neighborhood of an index.  We assume that the nearest
// point is on the interval [index, index+1].
//-----------------------------------------------------------------------------
bool bdgl_curve_algs::closest_point_near(vdgl_edgel_chain_sptr const& ec,
                                         const int index,
                                         const double x,
                                         const double y,
                                         double & xc,
                                         double & yc)
{
  int last = ec->size()-1;//last edgel
  if (index<0||index>last)
    return false;
  double t = 0;
  vnl_double_2 p(x, y);
  vnl_double_2 p0, p1, pc;
  if (index<last)
  {
    p0.x()=(*ec)[index].x();
    p0.y()=(*ec)[index].y();
    p1.x()=(*ec)[index+1].x();
    p1.y()=(*ec)[index+1].y();
  }
  if (index==last)
  {
    p0.x()=(*ec)[index-1].x();
    p0.y()=(*ec)[index-1].y();
    p1.x()=(*ec)[index].x();
    p1.y()=(*ec)[index].y();
  }
  t = interpolate_segment(p0, p1, p, pc);
  vcl_cout << "At " << p << " t = " << t << "\n";
  xc = pc.x();   yc = pc.y();
  return true;
}

//:
// It is sometimes necessary to reverse the order of the digital curve
// so that the initial point corresponds to v1 of a topology edge
vdgl_digital_curve_sptr bdgl_curve_algs::reverse(vdgl_digital_curve_sptr const& dc)
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
bool bdgl_curve_algs::intersect_line(vdgl_digital_curve_sptr const& dc,
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

//:
//Advance along a line and generate continguous pixels on a straight
// line defined by (xs, ys) : (xe, ye).  The samples are generated
// as values of (x, y).
// The routine is called in a loop that generates the points, e.g.,
//
//   while (line_gen(xs, ys, xe, ye, init, done, x, y))
//   { ...
// The routine needs two internal state variables, init and done.
// init should be set to true
// done should be set to false
// when the routine is first called.
//
bool bdgl_curve_algs::line_gen(const float xs, const float ys,
                               const float xe, const float ye,
                               bool& init, bool& done,
                               float& x, float& y)
{
  assert(xs >= 0.0f); assert(ys >= 0.0f);
  const float pix_edge = 1.0f; //We are working at scale = 1.0
  static float xi=0, yi=0;
  if (init)
  {
    xi = xs;
    yi = ys;
    x = (float)(xi/pix_edge);
    y = (float)(yi/pix_edge);
    init = false;
    return true;
  }
  if (done) return false;
  float dx = xe-xs;
  float dy = ye-ys;
  float mag = vcl_sqrt(dx*dx + dy*dy);
  if (mag<pix_edge)//Can't reach the next pixel under any circumstances
  {                //so just output the target, xe, ye.
    x = (float)xe; y = (float)ye;
    done = true;
    return true;
  }
  float delta = (0.5f*pix_edge)/mag; //move in 1/2 pixel increments
  //Previous pixel location
  int xp = int(xi/pix_edge);
  int yp = int(yi/pix_edge);
  //Increment along the line until the motion is greater than one pixel
  for (int i = 0; i<3; i++)
  {
    xi += dx*delta;
    yi += dy*delta;
    //Check for end of segment, make sure we emit the end of the segment
    if ((xe>=xs&&xi>xe)||(xe<=xs&&xi<xe)||(ye>=ys&&yi>ye)||(ye<=ys&&yi<ye))
    {
      x = (float)xe; y = (float)ye;
      done = true;
      return true;
    }
    //Check if we have advanced by more than .5 pixels
    x = (float)(xi/pix_edge);
    y = (float)(yi/pix_edge);
    if (vcl_abs(int(x)-xp)>(.5*pix_edge)||vcl_abs(int(y)-yp)>(.5*pix_edge))
      return true;
  }
  vcl_cout << "in bdgl_curve_algs::line_gen - shouldn't happen\n";
  return false;
}

//:
// Given an existing edgel_chain, add new edgels from the end
// along a straight digital line to reach (x, y). The number of
// edgels added is returned
int bdgl_curve_algs::add_straight_edgels(vdgl_edgel_chain_sptr const& ec,
                                         const double xe, const double ye,
                                         bool debug)
{
  if (!ec)
  {
    vcl_cout << "In bdgl_curve_algs::add_straight_edgels -"
             << " null edgel chain\n";
    return 0;
  }

  int Npix = 0, last = ec->size()-1;
  if (last<0)
  {
    vcl_cout << "In bdgl_curve_algs::add_straight_edgels -"
             << " chain with 0 edgels with target ("
             << xe << " " << ye << ")\n";
    return 0;
  }

  double xs = (*ec)[last].x(), ys = (*ec)[last].y();
  bool first = true, init = true, done = false;
  float x, y;
  while (bdgl_curve_algs::line_gen(xs, ys, xe, ye, init, done, x, y))
    if (!first)
    {
      vdgl_edgel ed(x, y, bdgl_curve_algs::synthetic);
      ec->add_edgel(ed);
      Npix++;
      if (debug)
        vcl_cout << "Adding edgel " << ed <<  "\n";
    }
    else
      first = false;//skip first point since it is already last element of ec
  return Npix;
}

//:
// returns either 0 or N depending on which end of the chain is
// closer to the given point
int bdgl_curve_algs::closest_end(vdgl_edgel_chain_sptr const & ec,
                                 const double x, const double y)
{
  if (!ec)
  {
    vcl_cout << "In bdgl_curve_algs::closest_end -"
             << " null edgel chain\n";
    return 0;
  }
  int N = ec->size();
  if (!N)
  {
    vcl_cout << "In bdgl_curve_algs::closest_end -"
             << " no edgels in chain\n";
    return 0;
  }
  double x0 = (*ec)[0].x(), y0=(*ec)[0].y();
  double xn = (*ec)[N-1].x(), yn=(*ec)[N-1].y();
  double d0 = vcl_sqrt((x0-x)*(x0-x)+ (y0-y)*(y0-y));
  double dn = vcl_sqrt((xn-x)*(xn-x)+ (yn-y)*(yn-y));
  if (d0<dn)
    return 0;
  return N;
}
