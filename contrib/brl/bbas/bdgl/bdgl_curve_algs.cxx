//:
// \file
#include <vcl_cmath.h>
#include <vcl_cstdlib.h> // for std::abs(int)
#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_math.h> // for pi
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#include <vsol/vsol_box_2d.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_interpolator_linear.h>
#include <vdgl/vdgl_digital_curve.h>
#include <bdgl/bdgl_curve_algs.h>
#include <bdgl/bdgl_tracker_curve.h>
#include <vnl/algo/vnl_gaussian_kernel_1d.h>

const double bdgl_curve_algs::tol = 1e-16;
const double bdgl_curve_algs::max_edgel_sep = 2.0; // the maximum seperation
                                                   // of edgels (in pixels)
const double bdgl_curve_algs::synthetic = 0;//Indicates synthetic edgel
                                            //default constructor is -1

//: Destructor
bdgl_curve_algs::~bdgl_curve_algs()
{
}

//:
//--------------------------------------------------------------------------
// Finds the index (on the interval [0.0 1.0]) on a digital curve closest
// to the given point (x, y).
//--------------------------------------------------------------------------

double bdgl_curve_algs::closest_point(vdgl_digital_curve_sptr const& dc,
                                      const double x, const double y)
{
  if (!dc)
  {
    vcl_cout<<"In bdgl_curve_algs::closest_point(..) -"
            << " warning, null digital curve\n";
    return 0;
  }
  vdgl_interpolator_sptr interp = dc->get_interpolator();
  vdgl_edgel_chain_sptr  ec = interp->get_edgel_chain();
  int index = closest_point(ec, x, y);
  double parm = index/ec->size();
  return parm;
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

//: Interpolates between p0 and p1 finding the closest point to p.
//  Returns the parameter t on [0, 1] -> [p0, p1].
static double interpolate_segment(vnl_double_2& p0,
                                  vnl_double_2& p1,
                                   vnl_double_2& p,
                                   vnl_double_2& pc)
{
  double Dx = p1[0]-p0[0], Dy = p1[1]-p0[1];
  double dx = p[0]-p0[0], dy = p[1]-p0[1];
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
  pc[0] = t*Dx + p0[0];   pc[1] = t*Dy + p0[1];
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
  vnl_double_2 p(x, y);
  vnl_double_2 p0, p1, pc;
  if (index<last)
  {
    p0[0]=(*ec)[index].x();
    p0[1]=(*ec)[index].y();
    p1[0]=(*ec)[index+1].x();
    p1[1]=(*ec)[index+1].y();
  }
  if (index==last)
  {
    p0[0]=(*ec)[index-1].x();
    p0[1]=(*ec)[index-1].y();
    p1[0]=(*ec)[index].x();
    p1[1]=(*ec)[index].y();
  }
  double t = interpolate_segment(p0, p1, p, pc);
  vcl_cout << "At " << p << " t = " << t << '\n';
  xc = pc[0];   yc = pc[1];
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
//: preliminary test to see if an infinite line intersects the bounding box of the digital curve.
bool bdgl_curve_algs::intersect_bounding_box(vdgl_digital_curve_sptr const& dc,
                                             vgl_line_2d<double> & line)
{
  vsol_box_2d_sptr bb = dc->get_bounding_box();
  if (!bb)
    return false;
  vgl_box_2d<double> box(bb->get_min_x(), bb->get_max_x(),
                         bb->get_min_y(), bb->get_max_y());
  vgl_point_2d<double> p0, p1;
  if (box.intersect(line, p0, p1))
    return true;
  return false;
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
  return vcl_fabs(inter[2]) >= bdgl_curve_algs::tol;
}


//: Recursive helper function for intersect_line_fast
static bool intersect_line_helper(vdgl_edgel_chain const& ec,
                                  vnl_double_3& lv,
                                  double dist1, double dist2,
                                  int index1, int index2,
                                  vcl_vector<vgl_point_2d<double> >& pts)
{
  int di = (index2 - index1);
  if (di<1){
    vcl_cout << "In bdgl_curve_algs::intersect_line_helper -"
             << " invalid curve segment\n";
    return false;
  }

  if (vcl_fabs(dist2)<bdgl_curve_algs::tol || dist1*dist2<0.0){
    // base case: compute the intersection
    if (di==1){
      // the first and last edgels
      vdgl_edgel const& e1 = ec[index1];
      vdgl_edgel const& e2 = ec[index2];
      vnl_double_3 p1(e1.get_x(), e1.get_y(), 1.0);
      vnl_double_3 p2(e2.get_x(), e2.get_y(), 1.0);

      vnl_double_3 inter;
      if (intersect_crossing(lv, p1, p2, inter)){
        vgl_point_2d<double> p(inter[0]/inter[2], inter[1]/inter[2]);
        pts.push_back(p);
        return true;
      }
      return false;
    }
  }
  else{
    if (di==1) return false;
    if ((di*bdgl_curve_algs::max_edgel_sep)<vcl_fabs(dist1+dist2)) return false;
  }

  int mid_index = index1 + di/2;
  vnl_double_3 mid_point(ec[mid_index].get_x(), ec[mid_index].get_y(), 1.0);
  double mid_dist = dot_product(mid_point, lv);
  bool i1 = intersect_line_helper(ec, lv, dist1, mid_dist, index1, mid_index, pts);
  bool i2 = intersect_line_helper(ec, lv, mid_dist, dist2, mid_index, index2, pts);
  return i1 || i2;
}

//-------------------------------------------------------------
//: intersect an infinite line with the digital curve.
//  If there is no intersection return false. Note that the line
//  can intersect multiple times. All the intersections are returned.
//
//  This implementation uses a recursive helper function
bool bdgl_curve_algs::intersect_line_fast(vdgl_digital_curve_sptr const& dc,
                                          vgl_line_2d<double> & line,
                                          vcl_vector<vgl_point_2d<double> >& pts)
{
  if (!dc)
  {
    vcl_cout << "In bdgl_curve_algs::intersect_line_fast - null curve\n";
    return false;
  }
  vdgl_interpolator_sptr interp = dc->get_interpolator();
  vdgl_edgel_chain_sptr  ec = interp->get_edgel_chain();

  // normalized the line so that the algebraic distance between
  // lv and (x,y,1) is the geometric distance
  line.normalize();
  vnl_double_3 lv(line.a(), line.b(), line.c());

  // the first and last edgels
  vdgl_edgel const& e1 = (*ec)[0];
  vdgl_edgel const& e2 = (*ec)[ec->size()-1];

  vnl_double_3 p1(e1.get_x(), e1.get_y(), 1.0);
  vnl_double_3 p2(e2.get_x(), e2.get_y(), 1.0);

  double dist1 = dot_product(p1, lv);
  double dist2 = dot_product(p2, lv);

  bool intersection = false;
  // This case (the first edgel is on the line)
  // is not covered by the recursion
  if (vcl_fabs(dist1)<bdgl_curve_algs::tol){
    vdgl_edgel const& e = (*ec)[1];
    vnl_double_3 p(e.get_x(), e.get_y(), 1.0);
    vnl_double_3 inter;
    if (intersect_crossing(lv, p1, p, inter)){
      vgl_point_2d<double> p(inter[0]/inter[2], inter[1]/inter[2]);
      pts.push_back(p);
      intersection = true;
    }
  }

  return intersect_line_helper(*ec, lv, dist1, dist2, 0, ec->size()-1, pts) ||
         intersection;
}


//-------------------------------------------------------------
//: intersect an infinite line with the digital curve.
//  If there is no intersection return false. Note that the line
//  can intersect multiple times. All the intersections are returned.
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
    p1[0]=dc->get_x(t); p1[1]=dc->get_y(t); p1[2]= 1.0;
    double sign0 = dot_product(p0, lv);
    double sign1 = dot_product(p1, lv);
    if (vcl_fabs(sign0)<bdgl_curve_algs::tol||              //we have crossed or
        vcl_fabs(sign1)<bdgl_curve_algs::tol||sign0*sign1<=0) // are on the line
    {
      vnl_double_3 inter;
      if (intersect_crossing(lv, p0, p1, inter))
      {
        vgl_point_2d<double> p(inter[0]/inter[2], inter[1]/inter[2]);
        pts.push_back(p);
        intersection = true;
      }
    }
    p0=p1;
  }
  return intersection;
}

//------------------------------------------------------------------
//:
// Given a line segment and a point interior to the line segment
// find the intermediate line parameter value for the point. t0 and t1
// are the line parameter values for p0 and p1 respectively.
static double interpolate_parameter(const double t0, const double t1,
                                    vnl_double_3& p0,
                                    vnl_double_3& p1,
                                    vnl_double_3& pt)
{
  if (vcl_fabs(p0[2])<bdgl_curve_algs::tol)
    return t0;
  if (vcl_fabs(p1[2])<bdgl_curve_algs::tol)
    return t0;
  if (vcl_fabs(pt[2])<bdgl_curve_algs::tol)
    return t0;
  double x0 = p0[0]/p0[2], y0 = p0[1]/p0[2];
  double x1 = p1[0]/p1[2], y1 = p1[1]/p1[2];
  double xt = pt[0]/pt[2], yt = pt[1]/pt[2];
  double d01 = vcl_sqrt((x1-x0)*(x1-x0) + (y1-y0)*(y1-y0));
  double d0t = vcl_sqrt((xt-x0)*(xt-x0) + (yt-y0)*(yt-y0));
  double r = d0t/d01;//relative length from p0 to pt
  double dt = t1-t0;
  return t0 + r*dt;
}


//: Recursive helper function for intersect_line_fast
static bool intersect_line_helper(vdgl_edgel_chain const& ec,
                                  vnl_double_3& lv,
                                  double dist1, double dist2,
                                  int index1, int index2,
                                  vcl_vector<double>& indices)
{
  int di = (index2 - index1);
  if (di<1){
    vcl_cout << "In bdgl_curve_algs::intersect_line_helper -"
             << " invalid curve segment\n";
    return false;
  }

  if (vcl_fabs(dist2)<bdgl_curve_algs::tol || dist1*dist2<0.0){
    // base case: compute the intersection
    if (di==1){
      // the first and last edgels
      vdgl_edgel const& e1 = ec[index1];
      vdgl_edgel const& e2 = ec[index2];
      vnl_double_3 p1(e1.get_x(), e1.get_y(), 1.0);
      vnl_double_3 p2(e2.get_x(), e2.get_y(), 1.0);

      vnl_double_3 inter;
      if (intersect_crossing(lv, p1, p2, inter)){
        double param_step = 1.0/(ec.size()-1);
        double ti = interpolate_parameter(index1*param_step, index2*param_step,
                                          p1, p2, inter);
        indices.push_back(ti);
        return true;
      }
      return false;
    }
  }
  else{
    if (di==1) return false;
    if ((di*bdgl_curve_algs::max_edgel_sep)<vcl_fabs(dist1+dist2)) return false;
  }

  int mid_index = index1 + di/2;
  vnl_double_3 mid_point(ec[mid_index].get_x(), ec[mid_index].get_y(), 1.0);
  double mid_dist = dot_product(mid_point, lv);

  bool i1 = intersect_line_helper(ec, lv, dist1, mid_dist, index1, mid_index, indices);
  bool i2 = intersect_line_helper(ec, lv, mid_dist, dist2, mid_index, index2, indices);
  return i1 || i2;
}

//-------------------------------------------------------------
//: intersect an infinite line with the digital curve.
//  If there is no intersection return false. Note that the line
//  can intersect multiple times. The curve parameter indices at
//  the intersection points are returned
//
//  This implementation uses a recursive helper function
bool bdgl_curve_algs::intersect_line_fast(vdgl_digital_curve_sptr const& dc,
                                          vgl_line_2d<double> & line,
                                          vcl_vector<double>& indices)
{
  if (!dc)
  {
    vcl_cout << "In bdgl_curve_algs::intersect_line_fast - null curve\n";
    return false;
  }
  vdgl_interpolator_sptr interp = dc->get_interpolator();
  vdgl_edgel_chain_sptr  ec = interp->get_edgel_chain();

  // normalized the line so that the algebraic distance between
  // lv and (x,y,1) is the geometric distance
  line.normalize();
  vnl_double_3 lv(line.a(), line.b(), line.c());

  // the first and last edgels
  vdgl_edgel const& e1 = (*ec)[0];
  vdgl_edgel const& e2 = (*ec)[ec->size()-1];

  vnl_double_3 p1(e1.get_x(), e1.get_y(), 1.0);
  vnl_double_3 p2(e2.get_x(), e2.get_y(), 1.0);

  double dist1 = dot_product(p1, lv);
  double dist2 = dot_product(p2, lv);

  bool intersection = false;
  // This case (the first edgel is on the line)
  // is not covered by the recursion
  if (vcl_fabs(dist1)<bdgl_curve_algs::tol){
    vdgl_edgel const& e = (*ec)[1];
    vnl_double_3 p(e.get_x(), e.get_y(), 1.0);
    vnl_double_3 inter;
    if (intersect_crossing(lv, p1, p, inter)){
      double param_step = 1.0/(ec->size()-1);
      double ti = interpolate_parameter(0.0, param_step, p1, p, inter);
      indices.push_back(ti);
      intersection = true;
    }
  }

  return intersect_line_helper(*ec, lv, dist1, dist2, 0, ec->size()-1, indices) ||
         intersection;
}


//-------------------------------------------------------------
//: intersect an infinite line with the digital curve.
//  If there is no intersection return false. Note that the line
//  can intersect multiple times. The curve parameter indices at
//  the intersection points are returned
bool bdgl_curve_algs::intersect_line(vdgl_digital_curve_sptr const& dc,
                                     vgl_line_2d<double>& line,
                                     vcl_vector<double>& indices)
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
    p1[0]=dc->get_x(t); p1[1]=dc->get_y(t); p1[2]= 1.0;
    double sign0 = dot_product(p0, lv);
    double sign1 = dot_product(p1, lv);
    if (vcl_fabs(sign0)<bdgl_curve_algs::tol||              //we have crossed or
        vcl_fabs(sign1)<bdgl_curve_algs::tol||sign0*sign1<=0) // are on the line
    {
      vnl_double_3 inter;
      if (intersect_crossing(lv, p0, p1, inter))
      {
        double ti = interpolate_parameter(t, t+dt, p0, p1, inter);
        indices.push_back(ti);
        intersection = true;
      }
    }
    p0=p1;
  }
  return intersection;
}


//:Intersect a curve and find the closest point to \a ref_point with a compatible gradient angle
bool
bdgl_curve_algs::match_intersection(vdgl_digital_curve_sptr const& dc,
                                    vgl_line_2d<double>& line,
                                    vgl_point_2d<double> const& ref_point,
                                    double ref_gradient_angle,
                                    vgl_point_2d<double>& point)
{
  double angle_thresh = 7.0;//epipolar angle threshold
  double angle_tol = 12.5;//gradient angle threshold
  if (!dc)
    return false;
  double la = line.slope_degrees();
  if (la<0)
    la+=180;
  vcl_vector<double> indices;
  if (!bdgl_curve_algs::intersect_line(dc, line, indices))
    return false;
  vgl_homg_point_2d<double> rph(ref_point.x(), ref_point.y());
  double dist = 1e10;
  bool found_valid_intersection = false;
  for (vcl_vector<double>::iterator iit = indices.begin();
       iit != indices.end(); iit++)
  {
    double grad_angle = dc->get_theta(*iit);
    if (vcl_fabs(ref_gradient_angle-grad_angle)>angle_tol)
      continue;
    double ca = dc->get_tangent_angle(*iit);
    if (ca<0)
      ca+=180;
    double delt = vcl_fabs(180*vcl_sin(vcl_fabs(vnl_math::pi*(ca-la)/180.0))/vnl_math::pi);
    if (delt<angle_thresh)
      continue;
    vgl_homg_point_2d<double> ph(dc->get_x(*iit), dc->get_y(*iit));
    double d = vgl_homg_operators_2d<double>::distance_squared(rph, ph);
    d = vcl_sqrt(d);
    found_valid_intersection = true;
    if (d<dist)
    {
      //double best_delt = delt;
      //double best_ind = *iit;
      dist = d;
      point = vgl_point_2d<double>(dc->get_x(*iit), dc->get_y(*iit));
    }
  }
  return found_valid_intersection;
}

//: generate contiguous pixels on a straight line.
// Advance along a line and generate contiguous pixels on a straight
// line defined by (xs, ys) : (xe, ye).  The samples are generated
// as values of (x, y).
// The routine is called in a loop that generates the points, e.g.,
// \code
//   while (line_gen(xs, ys, xe, ye, init, done, x, y))
//   { ...
// \endcode
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
    if (2*vcl_abs(int(x)-xp)>pix_edge || 2*vcl_abs(int(y)-yp)>pix_edge)
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
    vcl_cout << "In bdgl_curve_algs::add_straight_edgels - null edgel chain\n";
    return 0;
  }

  int Npix = 0, last = ec->size()-1;
  if (last<0)
  {
    vcl_cout << "In bdgl_curve_algs::add_straight_edgels - chain with 0 edgels with target ("
             << xe << ' ' << ye << ")\n";
    return 0;
  }

  float xs = float((*ec)[last].x()), ys = float((*ec)[last].y());
  bool first = true, init = true, done = false;
  float x, y;
  while (bdgl_curve_algs::line_gen(xs, ys, float(xe), float(ye), init, done, x, y))
    if (!first)
    {
      vdgl_edgel ed(x, y, bdgl_curve_algs::synthetic);
      ec->add_edgel(ed);
      Npix++;
      if (debug)
        vcl_cout << "Adding edgel " << ed << '\n';
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
    vcl_cout << "In bdgl_curve_algs::closest_end - null edgel chain\n";
    return 0;
  }
  int N = ec->size();
  if (!N)
  {
    vcl_cout << "In bdgl_curve_algs::closest_end - no edgels in chain\n";
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


// smooth a curve by convolving x(s),y(s) with a gaussian filter
void bdgl_curve_algs::
smooth_curve(vcl_vector<vgl_point_2d<double> > &curve,double sigma)
{
  vnl_gaussian_kernel_1d gauss_1d(sigma);
  curve.insert(curve.begin(),curve[0]);
  curve.insert(curve.begin(),curve[0]);
  curve.insert(curve.begin(),curve[0]);

  curve.insert(curve.end(),curve[curve.size()-1]);
  curve.insert(curve.end(),curve[curve.size()-1]);
  curve.insert(curve.end(),curve[curve.size()-1]);
  double sum=gauss_1d[0];
  for (int i=1;i<gauss_1d.width();i++)
    sum+=2*gauss_1d[i];

  for (unsigned int i=3; i+3<curve.size(); ++i)
  {
    double x=curve[i-3].x()*gauss_1d[3] + curve[i-2].x()*gauss_1d[2]+
             curve[i-1].x()*gauss_1d[1] + curve[i  ].x()*gauss_1d[0]+
             curve[i+1].x()*gauss_1d[1] + curve[i+2].x()*gauss_1d[2]+
             curve[i+3].x()*gauss_1d[3];
    double y=curve[i-3].y()*gauss_1d[3] + curve[i-2].y()*gauss_1d[2]+
             curve[i-1].y()*gauss_1d[1] + curve[i  ].y()*gauss_1d[0]+
             curve[i+1].y()*gauss_1d[1] + curve[i+2].y()*gauss_1d[2]+
             curve[i+3].y()*gauss_1d[3];
    x/=sum;
    y/=sum;
    curve[i].set(x,y);
  }
}

vdgl_digital_curve_sptr  bdgl_curve_algs::
create_digital_curves(vcl_vector<vgl_point_2d<double> > & curve)
{
  vdgl_edgel_chain_sptr vec;
  vec= new vdgl_edgel_chain;
  for (unsigned int j=0; j<curve.size(); ++j)
  {
    vdgl_edgel el(curve[j].x(),curve[j].y(), 0,0 );
    vec->add_edgel(el);
  }
  vdgl_interpolator_sptr interp= new vdgl_interpolator_linear(vec);
  vdgl_digital_curve_sptr dc = new vdgl_digital_curve(interp);
  return dc;
}

double  bdgl_curve_algs::compute_transformed_euclidean_distance
                                           (bdgl_tracker_curve_sptr c1,
                                            bdgl_tracker_curve_sptr c2,
                                            vnl_matrix<double> R,
                                            vnl_matrix<double> T,
                                            double s,vcl_map<int,int> alignment)
{
  if (!c1.ptr() && !c2.ptr())
    return -1;
  vcl_vector<double> x1,y1,x2,y2,x1t,y1t;
  double xcen1=0,xcen2=0,ycen1=0,ycen2=0;
  double H[2]={0,0};
  vcl_map<int,int>::iterator iter1;
  vcl_cout<<'\n'<<c1->desc->points_.size()<<'\t'<<c2->desc->points_.size();
  for (iter1 = alignment.begin(); iter1!=alignment.end(); ++iter1)
  {
    x1.push_back(c1->desc->points_[(*iter1).first].x());
    y1.push_back(c1->desc->points_[(*iter1).first].y());
    x2.push_back(c2->desc->points_[(*iter1).second].x());
    y2.push_back(c2->desc->points_[(*iter1).second].y());
  }
  for (unsigned int j=0; j+1<x1.size(); ++j)
  {
    vcl_cout<<'\n'<<x1[j]<<'\t'<<x2[j];
    xcen1+=x1[j];
    ycen1+=y1[j];
    xcen2+=x2[j];
    ycen2+=y2[j];
  }
  xcen1/=alignment.size();
  ycen1/=alignment.size();
  xcen2/=alignment.size();
  ycen2/=alignment.size();

  for (unsigned int i=0; i<x1.size(); ++i)
  {
    x1[i]-=xcen1; // x2[i]-=xcen2;
    y1[i]-=ycen1; // y2[i]-=ycen2;
  }
  double dist=0;
  double X2[2]={0,0};
  double X1cen[2]={xcen1,ycen1};
  for (unsigned int i=0; i<x1.size(); ++i)
  {
    H[0]=x1[i];
    H[1]=y1[i];
    X2[0]=x2[i];
    X2[1]=y2[i];

    vnl_matrix<double> X  (H, 2,1);
    vnl_matrix<double> X2t(X2, 2,1);

    vnl_matrix<double> X1center(X1cen, 2,1);
    vnl_matrix<double> Xt=R*X+T+X1center-X2t;
    vcl_cout<<'\n'<<X2t(0,0)<<'\t'<<X2t(1,0)<<'\t'<<X(0,0)+X1center(0,0)<<'\t'<<X(1,0)+X1center(0,0);
    dist+=vcl_sqrt(Xt(0,0)*Xt(0,0)+Xt(1,0)*Xt(1,0));
  }
  dist/=alignment.size();
  return dist;
}
