// This is brl/bseg/bmrf/bmrf_network_builder.cxx
#include "bmrf_network_builder.h"
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
#include <vdgl/vdgl_digital_curve.h>
#include <vtol/vtol_edge_2d.h>
#include <bmrf/bmrf_epi_point.h>
#include <bmrf/bmrf_epi_seg.h>
#include <bmrf/bmrf_node.h>
#include <bmrf/bmrf_network.h>
#include <bmrf/bmrf_epipole.h>


//---------------------------------------------------------------
// Constructors
//
//----------------------------------------------------------------

//: constructor from a parameter block (the only way)
//
bmrf_network_builder::bmrf_network_builder(bmrf_network_builder_params& tp)
  : bmrf_network_builder_params(tp)
{
  network_valid_ = false;
  epi_.set(eu_, ev_);
  du_ = elu_-eu_;
  //alpha scales
  alpha_min_ = vcl_atan2((double)(elv_min_-ev_), (double)du_);
  double alpha_max = vcl_atan2((double)(elv_max_-ev_), (double)du_);
  // double temp = alpha_max-alpha_min_;
  alpha_inv_ = 1.0; // 1.0/temp; <- TEMPORARY CHANGE

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
bmrf_network_builder::~bmrf_network_builder()
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
void bmrf_network_builder::epi_coords(const double u, const double v,
                                      double& alpha, double& s) const
{
  vgl_point_2d<double> p(u, v);
  vgl_line_segment_2d<double> el(epi_, p);
  //intersection with x = elu line
  vgl_vector_2d<double> dir = el.direction();
  double ang = vcl_atan2(dir.y(), dir.x());
  alpha = ang; // (ang-alpha_min_)*alpha_inv_;  <- TEMPORARY CHANGE
  s = line_distance(el, p);
}

void bmrf_network_builder::set_image(vil_image_view<float> const& image)
{
  image_=image;
  network_valid_ = false;
}

void bmrf_network_builder::set_edges(int frame,
                                     vcl_vector<vtol_edge_2d_sptr> const& edges)
{
  edges_.clear();
  frame_ = frame;
  edges_ = edges;
  network_valid_ = false;
}

void bmrf_network_builder::init()
{
  network_ = new bmrf_network();
  network_->set_epipole(bmrf_epipole(epi_),0);
}

//: insure that the knots are stored in alpha increasing order.
//  this simplifies interpolation
static void rectify_order(bmrf_epi_seg_sptr & seg)
{
  int n = seg->n_pts();
  if (seg->p(0)->alpha()<seg->p(n-1)->alpha())
    return;
  //the order is backward so reverse the order
  vcl_vector<bmrf_epi_point_sptr> temp(n);
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
static void alpha_segment(vcl_vector<bmrf_epi_point_sptr> const& samples,
                          vcl_vector<bmrf_epi_seg_sptr>& epi_segs)
{
  int n = samples.size();
  if (n<3)
    return;
  vcl_vector<bmrf_epi_seg_sptr> segs;
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
  bmrf_epi_seg_sptr seg = new bmrf_epi_seg();
  for (int i = 0; i<n; i++)
    if (breaks[i])
    {
      segs.push_back(seg);
      seg = new bmrf_epi_seg();
      seg->add_point(samples[i]);
    }
    else
      seg->add_point(samples[i]);
  segs.push_back(seg);

  //filter out short segments.
  //rectify the order
  int min_length = 4;
  for (vcl_vector<bmrf_epi_seg_sptr>::iterator sit = segs.begin();
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
bool bmrf_network_builder::
extract_alpha_segments(vdgl_digital_curve_sptr const & dc,
                       vcl_vector<bmrf_epi_seg_sptr>& epi_segs)
{
  if (!dc)
    return false;
  vcl_vector<bmrf_epi_point_sptr> samples;
  // make the increment about 1 pixel on the curve
  double dsp = 1.0/dc->length();
  for (double sp = 0.0; sp<1.0; sp+=dsp)
  {
    double u = dc->get_x(sp), v = dc->get_y(sp), alpha = 0, s =0;
    this->epi_coords(u, v, alpha, s);
    bmrf_epi_point_sptr ep =
      new bmrf_epi_point(u, v,
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
bmrf_network_builder::inside_epipolar_wedge(vdgl_digital_curve_sptr const& dc)
{
  if (!dc)
    return false;
  vsol_box_2d_sptr bb = dc->get_bounding_box();
  if (!bb)
    return false;
  //Generate the limiting points
  double xmin = bb->get_min_x(), xmax = bb->get_max_x();
  double ymin = bb->get_min_y(), ymax = bb->get_max_y();

#ifdef BMRF_DEBUG // filter Only for VW
  vsol_box_2d_sptr temp = new vsol_box_2d();
  temp->add_point(680, 350);   temp->add_point(870, 450);
  if (!((*bb)<(*temp)))
    return false;
#endif
//END BMRF_DEBUG

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
static void set_min_index(vcl_vector<bmrf_epi_seg_sptr>& min_segs)
{
  int n = min_segs.size();
  for (int i = 0; i<n; i++)
    min_segs[i]->set_min_index(i);
}

//: save the max cache index in the epi_seg
// the vector should be sorted at this point.
static void set_max_index(vcl_vector<bmrf_epi_seg_sptr>& max_segs)
{
  int n = max_segs.size();
  for (int i = 0; i<n; i++)
    max_segs[i]->set_max_index(i);
}

//Gives a sort on increasing epipolar minimum segment position
static bool epi_seg_compare_min(bmrf_epi_seg_sptr const n0,
                                bmrf_epi_seg_sptr const n1)
{
  return n0->min_s() < n1->min_s();
}

//Gives a sort on increasing epipolar minimum segment position
static bool epi_seg_compare_max(bmrf_epi_seg_sptr const n0,
                                bmrf_epi_seg_sptr const n1)
{
  return n0->max_s() < n1->max_s();
}

bool bmrf_network_builder::compute_segments()
{
  vcl_vector<bmrf_epi_seg_sptr> min_segments, max_segments;
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
    vcl_vector<bmrf_epi_seg_sptr> epi_segs;
    if (!this->extract_alpha_segments((*cit), epi_segs))
      continue;
    // For later computations, the epi_segs are sorted according
    // to min and max s values.  This ordering enables efficient
    // searching for nearby segments.
    int k=0;
    for (vcl_vector<bmrf_epi_seg_sptr>::iterator sit = epi_segs.begin();
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
  min_epi_segs_ = min_segments;
  max_epi_segs_ = max_segments;
  return true;
}

//: the map between epipolar coordinates and image coordinates
//  returns false if the image coordinates are out of bounds
bool bmrf_network_builder::image_coords(const double a, const double s,
                                        double& u, double& v) const
{
  u = 0; v = 0;
  //unscale alpha
  double A = a; // a/alpha_inv_ + alpha_min_; <- TEMPORARY CHANGE
  //get  u,v relative to the epipole
  u = s*vcl_cos(A); v = s*vcl_sin(A);
  //add the epipole position
  u += eu_; v += ev_;
  if (!image_)
    return false;
  if (u<0||u>=image_.ni()-0.5)
    return false;
  if (v<0||v>=image_.nj()-0.5)
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
bool bmrf_network_builder::
intensity_candidates(bmrf_epi_seg_sptr const& seg,
                     vcl_vector<bmrf_epi_seg_sptr>& left_cand,
                     vcl_vector<bmrf_epi_seg_sptr>& right_cand)
{
  if (!seg)
    return false;
  vcl_vector<bmrf_epi_seg_sptr>& min_segs = min_epi_segs_;
  vcl_vector<bmrf_epi_seg_sptr>& max_segs = max_epi_segs_;
  int n = min_segs.size();//same set as max_segs_, different order
  if (n<2)
    return false;//can't get bounds with only one seg
  vcl_vector<bmrf_epi_seg_sptr> left_temp, right_temp;
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
  for (vcl_vector<bmrf_epi_seg_sptr>::iterator sit = left_temp.begin();
       sit != left_temp.end(); sit++)
    if ((*sit)->min_alpha()<a_max && (*sit)->max_alpha()> a_min)
    {
      found_alpha = true;
      left_cand.push_back(*sit);
    }
  for (vcl_vector<bmrf_epi_seg_sptr>::iterator sit = right_temp.begin();
       sit != right_temp.end(); sit++)
    if ((*sit)->min_alpha()<a_max && (*sit)->max_alpha()> a_min)
    {
      found_alpha = true;
      right_cand.push_back(*sit);
    }
  return some_candidates&&found_alpha;
}

//: the radius for intensity sampling
double bmrf_network_builder::radius(const double s)
{
  return (Ns_*s)/smax_;
}

#if 0 // This function is not being used
//=====================================================================
//: Tests if a segment has an alpha range that intersects [min_a, max_a]
//  Also returns the bounds of the intersection, [as, ae].
//=====================================================================
static bool inside_range(const double min_a, const double max_a,
                         bmrf_epi_seg_sptr const& seg,
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
#endif // 0

//:find the closest left bounding segment s value
double bmrf_network_builder::
find_left_s(const double a, const double s,
            vcl_vector<bmrf_epi_seg_sptr> const& cand)
{
  double r = radius(s);
  if (!cand.size())
    return s-r;
  //find the closest smaller value of s
  double ds_min = vnl_numeric_traits<double>::maxval;
  for (vcl_vector<bmrf_epi_seg_sptr>::const_iterator sit = cand.begin();
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
double bmrf_network_builder::
find_right_s(const double a, const double s,
             vcl_vector<bmrf_epi_seg_sptr> const& cand)
{
  double r = radius(s);
  if (!cand.size())
    return s+r;
  //find the closest larger value of s
  double ds_min = vnl_numeric_traits<double>::maxval;
  for (vcl_vector<bmrf_epi_seg_sptr>::const_iterator sit = cand.begin();
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

double bmrf_network_builder::ds(const double s)
{
  return s/smax_;
}

//: Find the average intensity for given s limits on a line of constant alpha
double bmrf_network_builder::scan_interval(const double a, const double sl,
                                           const double s)
{
  if (!image_)
    return 0;
  int n_samples = 0;
  double sum = 0;
  for (double si = sl; si<=s; si+=ds(si), ++n_samples)
  {
    double u, v;
    if (!image_coords(a, si, u, v))
      continue;
    int ui = (int)(u+0.5), vi = (int)(v+0.5);
    sum+=image_(ui, vi);
#if 0
    sum += brip_float_ops::bilinear_interpolation(image_, u+0.5, v+0.5);//JLM
#endif
  }
  if (!n_samples)
    return 0;
  return sum/n_samples;
}

//scan along the epipolar line to the left
double bmrf_network_builder::
scan_left(double a, double s, vcl_vector<bmrf_epi_seg_sptr> const& left_cand,
          double& ds)
{
  double sl = this->find_left_s(a, s, left_cand);
  ds = s-sl;
  return  scan_interval(a, sl, s);
}

//scan along the epipolar line to the right
double bmrf_network_builder::
scan_right(double a,double s, vcl_vector<bmrf_epi_seg_sptr> const& right_cand,
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
bool bmrf_network_builder::fill_intensity_values(bmrf_epi_seg_sptr& seg)
{
#ifdef DEBUG
  vcl_cout << "\n\nStarting new Seg\n";
#endif
  //the potential bounding segments
  vcl_vector<bmrf_epi_seg_sptr> left_cand, right_cand;
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
bool bmrf_network_builder::set_intensity_info()
{
  //the min and max caches hold the same segments, just sorted
  //differently
  vcl_vector<bmrf_epi_seg_sptr>& segs = min_epi_segs_;
#ifdef DEBUG
  vcl_cout << "Intensity data for Frame " << frame_ << '\n';
#endif
  for (vcl_vector<bmrf_epi_seg_sptr>::iterator sit = segs.begin();
       sit != segs.end(); sit++)
  {
    this->fill_intensity_values(*sit);
#ifdef DEBUG
    vcl_cout << *(*sit) << vcl_endl;
#endif
  }
return true;
}

//==============================================================
//: Add the current nodes to the network
//=============================================================
bool bmrf_network_builder::add_frame_nodes()
{
  if (!network_)
    return false;
  vcl_vector<bmrf_epi_seg_sptr>& segs = min_epi_segs_;
  for (vcl_vector<bmrf_epi_seg_sptr>::iterator sit = segs.begin();
       sit != segs.end(); sit++)
  {
    //for now, make the node the entire alpha segment.
    bmrf_node_sptr node = new bmrf_node(*sit, frame_);
    if (!network_->add_node(node))
    {
      vcl_cout << "In bmrf_network_builder::build_network() -"
               << " trying to add a node that already exists "
               << *(*sit) << vcl_endl;
      return false;
    }
  }
  return true;
}

//==============================================================
//: Find the neighbors of a node in time from the previous frame
//  This algorithm is n^2 since for each node,
//  we have to scan the entire frame to find neighbors. But let's
//  keep it simple for now, until we get the whole process in place.
//=============================================================
bool bmrf_network_builder::
time_neighbors(bmrf_node_sptr const& node,
               vcl_vector<bmrf_node_sptr>& neighbors)
{
  if (!node)
    return false;

  //Get the s bounds information for the epi_seg in node
  double s_min = node->epi_seg()->min_s(), s_max = node->epi_seg()->max_s();
  //scan the frame for s bounds
  //the upper bound passes zero velocity
  //the lower bound is extended by the maximum velocity/depth range, gamma_max_.
  vcl_vector<bmrf_node_sptr> temp;
  int frame = node->frame_num();
  for (bmrf_network::seg_node_map::const_iterator nit = network_->begin(frame-1);
       nit != network_->end(frame-1); ++nit)
    if ((nit->first->min_s() <= s_max) &&
        (nit->first->max_s() > (s_min/(1.0+gamma_max_))))
      temp.push_back(nit->second);

  //filter out nodes that do not lie within the alpha range of the node
  //under consideration.
  double a_min = node->epi_seg()->min_alpha(),
    a_max = node->epi_seg()->max_alpha();

  bool found_something = false;
  for (vcl_vector<bmrf_node_sptr>::iterator nit = temp.begin();
       nit != temp.end(); nit++)
    if ((*nit)->epi_seg()->min_alpha()<a_max &&
        (*nit)->epi_seg()->max_alpha()> a_min)
    {
      found_something = true;
      neighbors.push_back(*nit);
    }

  return found_something;
}

//==============================================================
//: Assign neighbors to nodes.  For now just select a range of
//  neighbors from alpha and s in the previous frame, i.e.,
//  no in-frame neighbors.
//=============================================================
bool bmrf_network_builder::assign_neighbors()
{
  //If we are in the first frame, do nothing.
  if (!frame_)
    return true;
  //iterate over nodes in the current frame
  if (!network_)
    return false;

  for (bmrf_network::seg_node_map::const_iterator nit = network_->begin(frame_);
       nit != network_->end(frame_); ++nit )
  {
    vcl_vector<bmrf_node_sptr> neighbors;
    if (!this->time_neighbors(nit->second, neighbors))
      continue;
    for (vcl_vector<bmrf_node_sptr>::iterator nnit = neighbors.begin();
         nnit != neighbors.end(); nnit++)
    {
      network_->add_arc(nit->second, *nnit, bmrf_node::TIME);
      network_->add_arc(*nnit, nit->second, bmrf_node::TIME);
    }
  }
  return true;
}

bool bmrf_network_builder::build_network()
{
  if (!this->add_frame_nodes())
    return false;
  if (!this->assign_neighbors())
    return false;
  network_valid_ = true;
  return true;
}

//==============================================================
//: The main process method
//=============================================================
bool bmrf_network_builder::build()
{
  if (!this->compute_segments())
    return false;
  if (!this->set_intensity_info())
    return false;
  if (!this->build_network())
    return false;
  return true;
}

//: return the network if valid, otherwise a null network
bmrf_network_sptr bmrf_network_builder::network()
{
  bmrf_network_sptr net;
  if (!network_valid_)
    return net;
  return network_;
}
