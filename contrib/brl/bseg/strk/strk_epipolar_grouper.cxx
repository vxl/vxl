// This is brl/bseg/strk/strk_epipolar_grouper.cxx
#include "strk_epipolar_grouper.h"
//:
// \file
#include <vcl_cmath.h> // for vcl_fabs(double)
#include <vcl_algorithm.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_numeric_traits.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_line_segment_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vtol/vtol_edge_2d.h>
#include <brip/brip_vil1_float_ops.h>
#include <strk/strk_epi_point.h>
#include <strk/strk_epi_seg.h>


//---------------------------------------------------------------
// Constructors
//
//----------------------------------------------------------------

//: constructor from a parameter block (the only way)
//
strk_epipolar_grouper::strk_epipolar_grouper(strk_epipolar_grouper_params& tp)
  : strk_epipolar_grouper_params(tp)
{
  epi_.set(eu_, ev_);
  du_ = elu_-eu_;
  //alpha scales
  alpha_min_ = vcl_atan2((double)(elv_min_-ev_), (double)du_);
  double alpha_max = vcl_atan2((double)(elv_max_-ev_), (double)du_);
  double temp = alpha_max-alpha_min_;
  alpha_inv_ = 1.0/temp;

  //maximum s (distance from epipole).
  smax_ = vcl_sqrt(du_*du_+(elv_max_-ev_)*(elv_max_-ev_));

  //the increment in alpha is one pixel at the maximum s.
  double temp1 = vcl_atan2((double)(elv_max_+1-ev_), (double)du_);
  da_ = (temp1-alpha_max)*alpha_inv_;

  //upper wedge limit
  vgl_point_2d<double> upper(elu_, elv_max_);
  vgl_line_2d<double> ul(epi_, upper);
  upper_wedge_line_[0]=(ul.a());
  upper_wedge_line_[1]=(ul.b());
  upper_wedge_line_[2]=(ul.c());
  //lower wedge limit
  vgl_point_2d<double> lower(elu_, elv_min_);
  vgl_line_2d<double> ll(epi_, lower);
  lower_wedge_line_[0]=(ll.a());
  lower_wedge_line_[1]=(ll.b());
  lower_wedge_line_[2]=(ll.c());
}

//:Default Destructor
strk_epipolar_grouper::~strk_epipolar_grouper()
{
}

//: get the distance along a line from p0  to p
static double line_distance(vgl_line_segment_2d<double> const& seg,
                            vgl_point_2d<double> p)
{
  vgl_point_2d<double> p0 = seg.point1();
  vgl_vector_2d<double> v0(p0.x(), p0.y());
  vgl_vector_2d<double> v(p.x(), p.y());
  //get the length of p projected on  seg
  vgl_vector_2d<double> dir = seg.direction();
  vgl_vector_2d<double> dv = v-v0;
  double length = dot_product(dv,dir);
  return length;
}

//:conversion from image coordinates to epipolar coordinates
void strk_epipolar_grouper::epi_coords(const double u, const double v,
                                       double& alpha, double& s)
{
  vgl_point_2d<double> p(u, v);
  vgl_line_segment_2d<double> el(epi_, p);
  //intersection with x = elu line
  vgl_vector_2d<double> dir = el.direction();
  double ang = vcl_atan2(dir.y(), dir.x());
  alpha = (ang-alpha_min_)*alpha_inv_;
  s = line_distance(el, p);
}

void
strk_epipolar_grouper::set_edges(int frame,
                                 vcl_vector<vtol_edge_2d_sptr> const & edges)

{
  frame_ = frame;
  edges_ = edges;
}

void strk_epipolar_grouper::init(const int n_frames)
{
  min_epi_segs_.resize(n_frames);
  max_epi_segs_.resize(n_frames);
}

//: insure that the knots are stored in alpha increasing order.
//  this simplifies interpolation
static void rectify_order(strk_epi_seg_sptr & seg)
{
  int n = seg->n_pts();
  if (seg->p(0)->alpha()<seg->p(n-1)->alpha())
    return;
  //the order is backward so reverse the order
  vcl_vector<strk_epi_point_sptr> temp(n);
  for (int i = 0; i<n; i++)
    temp[n-i-1]= seg->p(i);
  //replace the points in the neigborhood
  seg->clear();
  for (int i = 0; i<n; i++)
    seg->add_point(temp[i]);
}

//==================================================================
//:find epipolar tangents and segment the curve into regions
// of monotonic alpha.  Note for now that the tangent points are
// removed from the segment. Need a separate representation for the tangents
//====================================================================
static void alpha_segment(vcl_vector<strk_epi_point_sptr> const& samples,
                          vcl_vector<strk_epi_seg_sptr>& epi_segs)
{
  int n = samples.size();
  if (n<3)
    return;
  vcl_vector<strk_epi_seg_sptr> segs;
  vcl_vector<bool> breaks;
  breaks.resize(n, false);
  double last_alpha = samples[1]->alpha();
  double dir = last_alpha-samples[0]->alpha();
  for (int i = 2; i<n; i++)
  {
    double alpha = samples[i]->alpha();
    double temp = alpha-last_alpha;
    last_alpha = alpha;
    //check for break (a reversal in direction)
    if (dir*temp<0)
    {
      dir = -dir;
      breaks[i]=true;
    }
  }
  strk_epi_seg_sptr seg = new strk_epi_seg();
  for (int i = 0; i<n; i++)
    if (breaks[i])
    {
      segs.push_back(seg);
      seg = new strk_epi_seg();
      seg->add_point(samples[i]);
    }
    else
      seg->add_point(samples[i]);
  segs.push_back(seg);

  //filter out short segments.
  //rectify the order
  int min_length = 4;
  for (vcl_vector<strk_epi_seg_sptr>::iterator sit = segs.begin();
       sit != segs.end(); sit++)
    if ((*sit)->n_pts()>min_length)
    {
      rectify_order(*sit);
      epi_segs.push_back(*sit);
    }
}

//==================================================================
//:Scan the input digital curve and produce monotonic alpha sequences
//==================================================================
bool strk_epipolar_grouper::
extract_alpha_segments(vdgl_digital_curve_sptr const & dc,
                       vcl_vector<strk_epi_seg_sptr>& epi_segs)
{
  if (!dc)
    return false;
  vcl_vector<strk_epi_point_sptr> samples;
  // make the increment about 1 pixel on the curve
  double dsp = 1.0/dc->length();
  for (double sp = 0.0; sp<1.0; sp+=dsp)
  {
    double u = dc->get_x(sp), v = dc->get_y(sp), alpha = 0, s =0;
    this->epi_coords(u, v, alpha, s);
    strk_epi_point_sptr ep =
      new strk_epi_point(u, v,
                         alpha,
                         s,
                         dc->get_grad(sp),
                         dc->get_theta(sp),
                         dc->get_tangent_angle(sp)
                        );
    samples.push_back(ep);
  }
  //segment the samples into monotonic alpha segments
  alpha_segment(samples, epi_segs);
  return true;
}

//======================================================================
//: Test if a digital curve is entirely inside the epipolar wedge
//  defined for processing.
//======================================================================
bool
strk_epipolar_grouper::inside_epipolar_wedge(vdgl_digital_curve_sptr const& dc)
{
  if (!dc)
    return false;
  vsol_box_2d_sptr bb = dc->get_bounding_box();
  if (!bb)
    return false;
  //Generate the limiting points
  double xmin = bb->get_min_x(), xmax = bb->get_max_x();
  double ymin = bb->get_min_y(), ymax = bb->get_max_y();
//   //DEBUG filter Only for VW
//   vsol_box_2d_sptr temp = new vsol_box_2d();
//   temp->add_point(680, 350);   temp->add_point(870, 450);
//   if (!((*bb)<(*temp)))
//     return false;
  //END DEBUG
  vnl_double_3 lower_left, lower_right;
  lower_left[0]=xmin;   lower_left[1]=ymin; lower_left[2]=1.0;
  lower_right[0]=xmax;  lower_right[1]=ymin; lower_right[2]=1.0;
  vnl_double_3 upper_left, upper_right;
  upper_left[0]=xmin;   upper_left[1]=ymax; upper_left[2]=1.0;
  upper_right[0]=xmax;  upper_right[1]=ymax; upper_right[2]=1.0;

  //:see if the box is below the upper wedge line
  bool below = dot_product(upper_left, upper_wedge_line_) < 0;
  below = below && dot_product(upper_right, upper_wedge_line_) < 0;

  //:see if the box is above the lower wedge line
  bool above = dot_product(lower_left, lower_wedge_line_) > 0;
  above = above && dot_product(lower_right, lower_wedge_line_) > 0;

  //: form inside
  bool inside = above&&below;
  if (inside)
    return true;
  return inside;
}

//: save the min cache index in the epi_seg
// the vector should be sorted at this point.
static void set_min_index(vcl_vector<strk_epi_seg_sptr>& min_segs)
{
  int n = min_segs.size();
  for (int i = 0; i<n; i++)
    min_segs[i]->set_min_index(i);
}

//: save the max cache index in the epi_seg
// the vector should be sorted at this point.
static void set_max_index(vcl_vector<strk_epi_seg_sptr>& max_segs)
{
  int n = max_segs.size();
  for (int i = 0; i<n; i++)
    max_segs[i]->set_max_index(i);
}

//Gives a sort on increasing epipolar minimum segment position
static bool epi_seg_compare_min(strk_epi_seg_sptr const n0,
                                strk_epi_seg_sptr const n1)
{
  return n0->min_s() < n1->min_s();
}

//Gives a sort on increasing epipolar minimum segment position
static bool epi_seg_compare_max(strk_epi_seg_sptr const n0,
                                strk_epi_seg_sptr const n1)
{
  return n0->max_s() < n1->max_s();
}

bool strk_epipolar_grouper::compute_segments()
{
  vcl_vector<strk_epi_seg_sptr> min_segments, max_segments;
  vcl_vector<vdgl_digital_curve_sptr> dcs;
  for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = edges_.begin();
       eit != edges_.end(); eit++)
    {
      vsol_curve_2d_sptr c = (*eit)->curve();
      vdgl_digital_curve_sptr dc = c->cast_to_digital_curve();
      if (!dc)
        continue;
      //:see if the curve is inside the epipolar wedge
     if (this->inside_epipolar_wedge(dc))
       dcs.push_back(dc);
    }
  for (vcl_vector<vdgl_digital_curve_sptr>::iterator cit = dcs.begin();
       cit != dcs.end(); cit++)
    {
      vcl_vector<strk_epi_seg_sptr> epi_segs;
      if (!this->extract_alpha_segments((*cit), epi_segs))
        continue;
      // For later computations, the epi_segs are sorted according
      // to min and max s values.  This ordering enables efficient
      // searching for nearby segments.
      int k=0;
      for (vcl_vector<strk_epi_seg_sptr>::iterator sit = epi_segs.begin();
           sit != epi_segs.end(); sit++, k++ )
        {
          min_segments.push_back(*sit);
          max_segments.push_back(*sit);
        }
    }
  //sort the segments according to min epipolar distance
  vcl_sort(min_segments.begin(),
           min_segments.end(),
           epi_seg_compare_min);
  set_min_index(min_segments);//store sorted index position in segs

  //sort the segments according to max epipolar distance
  vcl_sort(max_segments.begin(),
           max_segments.end(),
           epi_seg_compare_max);
  set_max_index(min_segments);//store sorted index position in segs

  //cache the segments for this frame.  Intensity info yet to be done
  min_epi_segs_[frame_] = min_segments;
  max_epi_segs_[frame_] = max_segments;
  return true;
}

//: the map between epipolar coordinates and image coordinates
//  returns false if the image coordinates are out of bounds
bool strk_epipolar_grouper::image_coords(const double a, const double s,
                                         double& u, double& v)
{
  u = 0; v = 0;
  if (!image_)
    return false;
  //unscale alpha
  double A = a/alpha_inv_ + alpha_min_;
  //get  u,v relative to the epipole
  u = s*vcl_cos(A); v = s*vcl_sin(A);
  //add the epipole position
  u += eu_; v += ev_;
  if (u<0||u>image_.width())
    return false;
  if (v<0||v>image_.height())
    return false;
  return true;
}

//==========================================================
//: fill in intensity information on an epi_seg.
// define r=ds*Ns. For the current seg:
//     alpha_min, alpha_max
//     s_min, s_max
// All intensity bound candidates, x,  must satisfy:
// x_alpha_min < alpha_max and x_alpha_max > alpha_min
//
// Candidates for the left scan must satisfy
// x_s_max > s_min - r
// x_s_min < s_max - r
//
// Candidates for the right scan also must satisfy
// x_s_min < s_max + r
// x_s_max > s_min + r
//
// Given this set of candidates, then for each (alpha, s) in the
// current seg, all the relevant candidates (left or right) are
// checked for the nearest bound. If there is a candidate, x, within
// r then the region is delimited by x.s(alpha), otherwise r.
//==============================================================
bool strk_epipolar_grouper::
intensity_candidates(strk_epi_seg_sptr const& seg,
                     vcl_vector<strk_epi_seg_sptr>& left_cand,
                     vcl_vector<strk_epi_seg_sptr>& right_cand)
{
  if (!seg)
    return false;
  vcl_vector<strk_epi_seg_sptr>& min_segs = min_epi_segs_[frame_];
  vcl_vector<strk_epi_seg_sptr>& max_segs = max_epi_segs_[frame_];
  int n = min_segs.size();//same set as max_segs_, different order
  if (n<2)
    return false;//can't get bounds with only one seg
  vcl_vector<strk_epi_seg_sptr> left_temp, right_temp;
  double a_min = seg->min_alpha(), a_max = seg->max_alpha();
  double s_min = seg->min_s(), s_max = seg->max_s();
  double r = s_min*Ns_;//scaled region radius
  //check bounds
  int min_index = seg->min_index();
  int max_index = seg->max_index();
  if (min_index<0||min_index>=n)
    return false;
  if (max_index<0||max_index>=n)
    return false;

  //scan the sorted arrays for suitable candidates
  //scan the min index to the left
  bool some_candidates = false;
  bool found_something = true;
  for (int im = min_index-1&&found_something; im>=0; im--)
  {
    if (min_segs[im]->min_s()< s_max-r)
    {
      left_temp.push_back(min_segs[im]);
      continue;
    }
    if (min_segs[im]->min_s() < s_max + r)
    {
      right_temp.push_back(min_segs[im]);
      continue;
    }
    found_something = false;
  }
  some_candidates = some_candidates || found_something;
  //scan the min index to the right
  found_something = true;
  for (unsigned int im = min_index+1; im<min_segs.size()&&found_something; ++im)
  {
    if (min_segs[im]->min_s()< s_max-r)
    {
      left_temp.push_back(min_segs[im]);
      continue;
    }
    if (min_segs[im]->min_s() < s_max + r)
    {
      right_temp.push_back(min_segs[im]);
      continue;
    }
    found_something = false;
  }
  some_candidates = some_candidates || found_something;
  //scan the max index to the left
  found_something = true;
  for (int im = max_index-1&&found_something; im>=0; im--)
  {
    if (max_segs[im]->max_s()< s_min-r)
    {
      left_temp.push_back(max_segs[im]);
      continue;
    }
    if (max_segs[im]->max_s() < s_min + r)
    {
      right_temp.push_back(min_segs[im]);
      continue;
    }
    found_something = false;
  }
  some_candidates = some_candidates || found_something;
  //scan the max index to the right
  found_something = true;
  for (unsigned int im = max_index+1; im<max_segs.size()&&found_something; --im)
  {
    if (max_segs[im]->max_s()< s_min-r)
    {
      left_temp.push_back(max_segs[im]);
      continue;
    }
    if (max_segs[im]->max_s() < s_min + r)
    {
      right_temp.push_back(min_segs[im]);
      continue;
    }
    found_something = false;
  }
  some_candidates = some_candidates || found_something;
  // At this point we have found all the viable candidates based on the
  // s dimension. Next we pass candidates based on the alpha range
  // x_alpha_min < alpha_max and x_alpha_max > alpha_min
  bool found_alpha = false;
  for (vcl_vector<strk_epi_seg_sptr>::iterator sit = left_temp.begin();
       sit != left_temp.end(); sit++)
    if ((*sit)->min_alpha()<a_max && (*sit)->max_alpha()> a_min)
    {
      found_alpha = true;
      left_cand.push_back(*sit);
    }
  for (vcl_vector<strk_epi_seg_sptr>::iterator sit = right_temp.begin();
       sit != right_temp.end(); sit++)
    if ((*sit)->min_alpha()<a_max && (*sit)->max_alpha()> a_min)
    {
      found_alpha = true;
      right_cand.push_back(*sit);
    }
  return found_something&&found_alpha;
}

//: the radius for intensity sampling
double strk_epipolar_grouper::radius(const double s)
{
  return (Ns_*s)/smax_;
}

//:find the closest left bounding segment s value
double strk_epipolar_grouper::
find_left_s(const double a, const double s,
            vcl_vector<strk_epi_seg_sptr> const& cand)
{
  double r = radius(s);
  if (!cand.size())
    return s-r;
  //find the closest smaller value of s
  double ds_min = vnl_numeric_traits<double>::maxval;
  for (vcl_vector<strk_epi_seg_sptr>::const_iterator sit = cand.begin();
       sit != cand.end(); sit++)
    {
      if (a<(*sit)->min_alpha()||a>(*sit)->max_alpha())
        continue;
      double xs = (*sit)->s(a);
      if (xs<s)
      {
        double ds = s-xs;
        ds_min = vnl_math_min(ds_min, ds);
      }
    }
  if (ds_min<r)
  {
    return s-ds_min;
  }
  return s-r;
}

//:find the closest right bounding segment s value
double strk_epipolar_grouper::
find_right_s(const double a, const double s,
             vcl_vector<strk_epi_seg_sptr> const& cand)
{
  double r = radius(s);
  if (!cand.size())
    return s+r;
  //find the closest larger value of s
  double ds_min = vnl_numeric_traits<double>::maxval;
  for (vcl_vector<strk_epi_seg_sptr>::const_iterator sit = cand.begin();
       sit != cand.end(); sit++)
    {
      if (a<(*sit)->min_alpha()||a>(*sit)->max_alpha())
        continue;
      double xs = (*sit)->s(a);
      if (xs>s)
      {
        double ds = xs-s;
        ds_min = vnl_math_min(ds_min, ds);
      }
    }
  if (ds_min<r)
  {
    return s+ds_min;
  }
  return s+r;
}

double strk_epipolar_grouper::ds(const double s)
{
  return s/smax_;
}

//: Find the average intensity for given s limits on a line of constant alpha
double strk_epipolar_grouper::scan_interval(const double a, const double sl,
                                            const double s)
{
   if (!image_)
    return 0;
  int n_samples = 0;
  double sum = 0;
  for (double si = sl; si<=s; si+=ds(si), n_samples++)
  {
    double u, v;
    if (!image_coords(a, si, u, v))
      continue;
    int ui = (int)(u+0.5), vi = (int)(v+0.5);
    sum+=image_(ui, vi);
#if 0
    sum += brip_vil1_float_ops::bilinear_interpolation(image_, u+0.5, v+0.5);//JLM
#endif
  }
  if (!n_samples)
    return 0;
  return sum/n_samples;
}

//scan along the epipolar line to the left
double strk_epipolar_grouper::
scan_left(double a, double s, vcl_vector<strk_epi_seg_sptr> const& left_cand,
          double& ds)
{
  double sl = this->find_left_s(a, s, left_cand);
  ds = s-sl;
  return  scan_interval(a, sl, s);
}

//scan along the epipolar line to the right
double strk_epipolar_grouper::
scan_right(double a,double s, vcl_vector<strk_epi_seg_sptr> const& right_cand,
           double& ds)
{
  double sr = this->find_right_s(a, s, right_cand);
  ds = sr-s;
  return  scan_interval(a, s, sr);
}

//==================================================================
//: scan the segment and sample intensity values in the region
//  bounded by the closest left and right segments or by the
//  limiting scale.
//==================================================================
bool strk_epipolar_grouper::fill_intensity_values(strk_epi_seg_sptr& seg)
{
  vcl_cout << "\n\nStarting new Seg\n";
  //the potential bounding segments
  vcl_vector<strk_epi_seg_sptr> left_cand, right_cand;
  this->intensity_candidates(seg, left_cand, right_cand);
  //scan the segment
  double min_a = seg->min_alpha(), max_a = seg->max_alpha();
  for (double a = min_a; a<=max_a; a+=da_)
  {
    double s = seg->s(a), left_int, right_int, left_ds=0, right_ds=0;
    left_int = scan_left(a, s, left_cand, left_ds);
    right_int = scan_right(a, s, right_cand, right_ds);
    seg->add_int_sample(a, left_ds, left_int, right_ds, right_int);
  }
return true;
}

//======================================================================
//: set the intensity data for the epi segments on this frame
//======================================================================
bool strk_epipolar_grouper::set_intensity_info()
{
  //the min and max caches hold the same segments, just sorted
  //differently
  vcl_vector<strk_epi_seg_sptr>& segs = min_epi_segs_[frame_];
  vcl_cout << "Intensity data for Frame " << frame_ << '\n';
  for (vcl_vector<strk_epi_seg_sptr>::iterator sit = segs.begin();
       sit != segs.end(); sit++)
  {
    this->fill_intensity_values(*sit);
    vcl_cout << *(*sit) << '\n'<< vcl_flush;
  }
return true;
}

//==============================================================
//: The main process method
//=============================================================
bool strk_epipolar_grouper::group()
{
  if (!this->compute_segments())
    return false;
  if (!this->set_intensity_info())
    return false;
  brute_force_match();
  return true;
}

vcl_vector<vsol_polyline_2d_sptr>
strk_epipolar_grouper::display_segs(int /* frame */)
{
  vcl_vector<vsol_polyline_2d_sptr> polys;
  vcl_vector<strk_epi_seg_sptr>& segs = min_epi_segs_[frame_];
  for (vcl_vector<strk_epi_seg_sptr>::iterator sit = segs.begin();
       sit != segs.end(); sit++)
  {
    vcl_vector<vsol_point_2d_sptr> points;
    int n = (*sit)->n_pts();
    for (int i = 0; i<n; i++)
    {
      strk_epi_point_sptr ep = (*sit)->p(i);
      vgl_point_2d<double> p = ep->p();
      vsol_point_2d_sptr sp = new vsol_point_2d(p);
      points.push_back(sp);
    }
    vsol_polyline_2d_sptr poly = new vsol_polyline_2d(points);
    polys.push_back(poly);
  }
  return polys;
}

//=====================================================================
//: Tests if a segment has an alpha range that intersects [min_a, max_a]
//  Also returns the bounds of the intersection, [as, ae].
//=====================================================================
static bool inside_range(const double min_a, const double max_a,
                         strk_epi_seg_sptr const& seg,
                         double& as, double& ae)
{
  if (!seg)
    return false;
  double seg_min_a = seg->min_alpha(), seg_max_a = seg->max_alpha();
  //test for inclusion
  if (seg_max_a < min_a)
    return false;
  if (seg_min_a > max_a)
    return false;
  //intersect segment alpha limits
  as = min_a; ae = max_a;
  if (as<seg_min_a)
    as = seg_min_a;
  if (ae>seg_max_a)
    ae = seg_max_a;
  return true;
}

//========================================================================
//: A brute force match algorithm for debugging
//=======================================================================
void strk_epipolar_grouper::brute_force_match()
{
  if (min_epi_segs_.size()!=2)
    return;
  vcl_vector<strk_epi_seg_sptr>& segs0 = min_epi_segs_[0];
  vcl_vector<strk_epi_seg_sptr>& segs1 = min_epi_segs_[1];
  for (vcl_vector<strk_epi_seg_sptr>::iterator sit0 = segs0.begin();
       sit0 != segs0.end(); sit0++)
    {
      double min_a = (*sit0)->min_alpha(), max_a = (*sit0)->max_alpha();
      double min_s = (*sit0)->s(min_a), max_s = (*sit0)->s(max_a);
      double u_min, v_min, u_max, v_max;
      image_coords(min_a, min_s, u_min, v_min);
      image_coords(max_a, max_s, u_max, v_max);
      vcl_cout << "\n\nMatching a: [" << u_min << ' ' << v_min << "]["
               << u_max << ' ' << v_max << "]\n";
      for (vcl_vector<strk_epi_seg_sptr>::iterator sit1 = segs1.begin();
           sit1 != segs1.end(); sit1++)
        {
          double as=0, ae=0;//alpha limit intersection
          if (!inside_range(min_a, max_a, (*sit1), as, ae))
            continue;
          double ss = (*sit1)->s(as), se = (*sit1)->s(ae);
          double us, vs, ue, ve;
          image_coords(as, ss, us, vs);
          image_coords(ae, se, ue, ve);
          vcl_cout << "\nto b: [" << us << ' ' << vs << "]["
                   << ue << ' ' << ve << "]\n"<< vcl_flush;
          for (double a = as; a<=ae; a+=da_)
          {
            strk_epi_seg::match(a, *sit0, *sit1);
          }
        }
    }
}

vil1_memory_image_of<unsigned char> strk_epipolar_grouper::epi_region_image()
{
  vil1_memory_image_of<unsigned char> out;
  if (!image_||!min_epi_segs_[frame_].size())
    return out;
  int w = image_.width(), h = image_.height();
  out.resize(w,h);
  for (int r = 0; r<h; r++)
    for (int c = 0; c<w; c++)
      out(c,r)=0;
  vcl_vector<strk_epi_seg_sptr>& segs = min_epi_segs_[frame_];
  for (vcl_vector<strk_epi_seg_sptr>::iterator sit = segs.begin();
       sit != segs.end(); sit++)
    {
      strk_epi_seg_sptr seg = (*sit);
      double amin = seg->min_alpha(), amax = seg->max_alpha();
      for (double a = amin; a<=amax; a+=da_)
      {
        double s0 = seg->s(a);
        double slmin = s0-seg->left_ds(a);
        double srmax = s0+seg->right_ds(a);
        unsigned char il = (unsigned char)seg->left_int(a);
        unsigned char ir = (unsigned char)seg->right_int(a);
        double u, v;
        int ui, vi;

        for (double s = slmin; s<s0; s++)
        {
          image_coords(a, s, u, v);
          ui = (int)(u+0.5), vi = (int)(v+0.5);
          if (out(ui,vi)==0)
            out(ui, vi) = il;
        }

        for (double s = s0; s<srmax; s++)
        {
          image_coords(a, s, u, v);
          ui = (int)(u+0.5), vi = (int)(v+0.5);
          if (out(ui,vi)==0)
            out(ui, vi) = ir;
        }
      }
    }
  return out;
}
