// This is brl/bseg/sdet/sdet_grid_finder.cxx
#include <fstream>
#include <iostream>
#include <algorithm>
#include "sdet_grid_finder.h"
//:
// \file
//
// \verbatim
//  Modifications
//  Bing Yu 1/18/2008
//    - removed unnecessary check on line length in compute_affine_homography()
//    - bug fixed: n_lines_x_/n_lines_y were switched when computing
//      grid offset in compute_homography_linear_chamfer(),
//      writing grid points in init_output_file(), and write_image_points()
//    - bug fixed in iterator over "squares" in a few places in
//      get_square_pixel_stats()
// \endverbatim

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_det.h>
#include <vbl/vbl_bounding_box.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_4point.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_linear.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <bsol/bsol_hough_line_index.h>
#include <bsol/bsol_algs.h>
#include <bsol/bsol_distance_histogram.h>
#include <vnl/algo/vnl_convolve.h>
#include <gevd/gevd_bufferxy.h>
static const double collection_grid_radius = 15;
static const double grid_radius = 15;

static void print_lines(std::vector<vsol_line_2d_sptr>& lines)
{
  for (auto & line : lines)
  {
    vgl_homg_line_2d<double> l = line->vgl_hline_2d();
    l.normalize();
    std::cout << l << '\n';
  }
}


// Gives a sort on signed distance of the
// line from the origin along the line normal.
// note that for line, hl, distance = -hl.c()
static bool line_distance(const vsol_line_2d_sptr & l1,
                          const vsol_line_2d_sptr & l2)
{
  vgl_homg_line_2d<double> hl1 = l1->vgl_hline_2d();
  vgl_homg_line_2d<double> hl2 = l2->vgl_hline_2d();
  hl1.normalize(); hl2.normalize();
  return hl1.c() > hl2.c();
}

#if 0
line_chamfer_1d::line_chamfer_1d()
{
  size_=0;
  dmin_=0;
  dmax_=0;
}

line_chamfer_1d::~line_chamfer_1d()
{
  for (int i = 0; i<size_; i++)
    delete line_index_[i];
}

bool line_chamfer_1d::insert_lines(std::vector<vsol_line_2d_sptr> const& lines,
                                   bool horizontal_lines)
{
  if (!lines.size())
    return false;
  dmin_ = vnl_numeric_traits<double>::maxval;
  dmax_ = -dmin_;
  distances_.clear();

  for (std::vector<vsol_line_2d_sptr>::const_iterator lit = lines.begin();
       lit != lines.end(); lit++)
  {
    vgl_homg_line_2d<double> l = (*lit)->vgl_hline_2d();
    l.normalize();
    double d = -l.c(); // perpendicular distance
    distances_.push_back(d);
    dmin_ = std::min(d,dmin_);
    dmax_ = std::max(d,dmax_);
  }
  if (!(dmax_>=dmin_))
    return false;
  size_=(int)(dmax_-dmin_ +1);
  index_.resize(size_, vnl_numeric_traits<int>::maxval);

  line_index_.resize(size_);
  for (int i = 0; i<size_; i++)
    line_index_[i]= new std::vector<vsol_line_2d_sptr>;

  int line = 0;
  for (std::vector<double>::iterator dit = distances_.begin();
       dit != distances_.end(); dit++, line++)
  {
    int id = (int)((*dit)-dmin_);
    index_[id]=0;
    line_index_[id]->push_back(lines[line]);
  }
  this->forward_champher();
  this->backward_champher();
#ifdef DEBUG
   for (int i = 0; i<size_; i++)
     std::cout << "line_index_["<<i+(int)dmin_<<"]="<<line_index_[i]->size() << '\n';
#endif
  return true;
}

void line_chamfer_1d::forward_champher()
{
  for (int i = 1; i<size_; i++)
    if (index_[i-1]+1<=index_[i])
      index_[i]=index_[i-1]+1;
}

void line_chamfer_1d::backward_champher()
{
  for (int i = size_-2; i>=0; i--)
    if (index_[i]>index_[i+1]+1)
      index_[i]=index_[i+1]+1;
}

double line_chamfer_1d::distance(double x) const
{
  if (x<dmin_)
    return vnl_numeric_traits<double>::maxval;
  if (x>dmax_)
    return vnl_numeric_traits<double>::maxval;
  int i = (int)(x-dmin_);
  return index_[i];
}

bool
line_chamfer_1d::get_lines_in_interval(const double x,
                                       const double radius,
                                       std::vector<vsol_line_2d_sptr>& lines) const
{
  lines.clear();
  if (x<dmin_)
    return false;
  if (x>dmax_)
    return false;
  double x0 = x - dmin_;
  int lo = (int)(x0 - radius);
  int hi = (int)(x0 + radius);
  if (lo<0)
    lo=0;
  if (hi>(size_-1))
    hi = size_ - 1;
  for (int d = lo; d<=hi; d++)
    if (line_index_[d])
      for (std::vector<vsol_line_2d_sptr>::iterator lit=line_index_[d]->begin();
           lit != line_index_[d]->end(); lit++)
        lines.push_back(*lit);
  return true;
}
#endif

// DEC - use total length of lines within radius instead of distance to nearest
grid_profile_matcher::grid_profile_matcher()
{
  size_=0;
  dmin_=0;
  dmax_=0;
}

grid_profile_matcher::~grid_profile_matcher()
{
  for (int i = 0; i<size_; i++)
    delete line_index_[i];
}

bool grid_profile_matcher::insert_lines(std::vector<vsol_line_2d_sptr> const& lines,
                                        bool horizontal_lines)
{
  if (!lines.size())
    return false;
  dmin_ = vnl_numeric_traits<double>::maxval;
  dmax_ = -dmin_;
  distances_.clear();
  for (const auto & line : lines)
  {
    double d;
#ifdef USE_C_FOR_DISTANCE
    vgl_homg_line_2d<double> l = (*lit)->vgl_hline_2d();
    l.normalize();
    d = -l.c(); // perpendicular distance
#else
    vsol_point_2d_sptr mid = line->middle();
    if (horizontal_lines)
      d = mid->y();
    else
      d = mid->x();
#endif
    distances_.push_back(d);
    dmin_ = std::min(d,dmin_);
    dmax_ = std::max(d,dmax_);
  }
  if (!(dmax_>=dmin_))
    return false;
  size_=(int)(dmax_-dmin_ +1);
  image_profile_.set_size(size_);
  image_profile_.fill(0.0);

  line_index_.resize(size_);
  for (int i = 0; i<size_; i++)
    line_index_[i]= new std::vector<vsol_line_2d_sptr>;

  int line = 0;
  for (auto dit = distances_.begin();
       dit != distances_.end(); dit++, line++)
  {
    int id = (int)((*dit)-dmin_);
    image_profile_[id] += lines[line]->length();
    line_index_[id]->push_back(lines[line]);
  }

  if (false)
  {
    if (horizontal_lines)
      std::cout << "------horizontal profile: ---------\n";
    else
      std::cout << "------vertical profile: -----------\n";
    for (int i = 0; i < size_; i++)
    {
      std::cout <<"profile["<<int(i+dmin_)<<"] =  "<<image_profile_[i]<<'\n';
    }
  }
  return true;
}

bool
grid_profile_matcher::get_lines_in_interval(const double x,
                                            const double radius,
                                            std::vector<vsol_line_2d_sptr>& lines) const
{
  lines.clear();
  if (x<dmin_)
    return false;
  if (x>dmax_)
    return false;
  double x0 = x - dmin_;
  int lo = (int)(x0 - radius);
  int hi = (int)(x0 + radius);
  if (lo<0)
    lo=0;
  if (hi>(size_-1))
    hi = size_ - 1;
  for (int d = lo; d<=hi; d++)
    if (line_index_[d])
      for (auto & lit : *line_index_[d])
        lines.push_back(lit);
  return true;
}

double grid_profile_matcher::calculate_grid_offset(int n_grid_lines, double spacing)
{
  // construct grid profile vector
  int grid_profile_len = int((n_grid_lines+1) * spacing) + 1;
  vnl_vector<double> grid_profile(grid_profile_len,0.0);
  int max_offset = int(grid_radius);
#ifdef DEBUG
  std::cout <<"max_offset = "<<max_offset<<'\n';
#endif
  for (int i = 1; i <= n_grid_lines; i++)
  {
    for (int offset = -max_offset; offset < max_offset; offset++)
    {
      grid_profile[int(i*spacing)+offset] = ((double)(max_offset -
                                                      std::abs(double(offset))))/(max_offset);
    }
  }
#ifdef DEBUG
    std::cout << "___GRID PROFILE__\n";
    for (int i = 0; i < grid_profile_len; i++)
      std::cout << "grid_profile["<<i<<"] = "<<grid_profile[i]<<'\n';
#endif
  // now convolve with image profile
  vnl_vector<double> convolution = vnl_convolve(image_profile_,grid_profile,0);
  // max value in convolution should correspond to best grid offset
  double max_value = -vnl_numeric_traits<double>::maxval;
  int max_index = -1;
#ifdef DEBUG
  std::cout << "__CONVOLUTION__\n";
#endif
  for (unsigned int i = 0; i < convolution.size(); i++)
  {
#ifdef DEBUG
    std::cout << "convolution["<<i-grid_profile_len+spacing+dmin_<<"] = "<<convolution[i]<<'\n';
#endif
    if (convolution[i] > max_value)
    {
      max_index = i;
      max_value = convolution[i];
    }
  }
  return max_index - grid_profile_len + spacing + dmin_;
}

//---------------------------------------------------------------
// Constructors
//
//----------------------------------------------------------------

//: constructor from a parameter block (the only way)
//
sdet_grid_finder::sdet_grid_finder(sdet_grid_finder_params& gfp)
  : sdet_grid_finder_params(gfp),
    length_threshold_(15.0)
{
  groups_valid_ = false;
  vanishing_points_valid_=false;
  projective_homography_valid_ = false;
  affine_homography_valid_ = false;
  homography_valid_ = false;
  index_ = nullptr;
}

//:Default Destructor
sdet_grid_finder::~sdet_grid_finder()
= default;

static void group_angle_stats(std::vector<vsol_line_2d_sptr> const & group,
                              const double angle_tol,
                              double & avg_angle, double& min_angle,
                              double& max_angle)
{
  min_angle = 360;
  max_angle = -360;
  int n_lines = 0;
  avg_angle=0;

  for (const auto & lit : group)
  {
    double ang = lit->tangent_angle();
    if (ang>180)
      ang -= 180.0;
    if (ang<min_angle)
      min_angle = ang;
    if (ang>max_angle)
      max_angle = ang;
  }
  // See if we are on the 180-0 cut
  double cut_thresh = 3.0*angle_tol + 15.0;
  bool on_cut = (max_angle-min_angle)>cut_thresh;
  if (on_cut)
    std::cout << "On cut [" << min_angle << ' ' << max_angle << "] >"
             << cut_thresh << '\n';
  for (auto lit = group.begin();
       lit != group.end(); lit++, n_lines++)
  {
    double ang = (*lit)->tangent_angle();
    if (ang>180.0)
      ang-=180.0;
    if (on_cut&&ang>90)
      ang -=180;
    avg_angle += ang;
  }
  if (n_lines)
  {
    avg_angle /= n_lines;
  }
  else
    avg_angle=0;
}

//-------------------------------------------------------------------------
//: Set the edges to be processed
//
bool sdet_grid_finder::set_lines(const float xsize, const float ysize,
                                 std::vector<vsol_line_2d_sptr> const& lines)
{
  debug_lines_.clear();
  debug_grid_lines_.clear();
  display_lines_.clear();
  matched_lines_.clear();
  groups_valid_ = false;
  vanishing_points_valid_=false;
  projective_homography_valid_=false;
  affine_homography_valid_ = false;
  homography_valid_=false;
  lines_=lines;
  xmax_ = xsize;
  ymax_ = ysize;
  index_ = new bsol_hough_line_index(0,0,xsize, ysize);
  for (const auto & line : lines)
    index_->index(line);
  std::vector<std::vector<vsol_line_2d_sptr> > groups;
  if (index_->dominant_line_groups(thresh_, angle_tol_, groups)<2)
    return false;
  group0_ = groups[0];
  group1_ = groups[1];
  groups_valid_ = true;
  if (debug_state_==VANISHING_POINT)
  {
    for (std::vector<vsol_line_2d_sptr>::const_iterator lit = groups[0].begin();
         lit != groups[0].end(); lit++)
      debug_lines_.push_back(*lit);
    for (std::vector<vsol_line_2d_sptr>::const_iterator lit = groups[1].begin();
         lit != groups[1].end(); lit++)
      debug_lines_.push_back(*lit);
  }
  return true;
}

bool sdet_grid_finder::
get_vanishing_point(std::vector<vsol_line_2d_sptr> const & para_lines,
                    vgl_homg_point_2d<double>& vp)
{
  std::vector<vgl_homg_line_2d<double> > vlines, tvlines, stvlines;

  int nlines =0;
  double tx = 0, ty =0;
  //
  // convert the input lines to vgl_homg_line(s)
  // and get the average distance of the intersection of the perpendicular
  // line from the origin with each line.  This point set defines the
  // translation offset for the lines.
  //
  for (const auto & para_line : para_lines)
  {
    if (para_line->length() < length_threshold_)
    {
      std::cout << "discarding line with length < "<<length_threshold_<<'\n';
      continue;
    }
    vgl_homg_line_2d<double> l= para_line->vgl_hline_2d();
    l.normalize();
    tx -= l.a()*l.c();
    ty -= l.b()*l.c();
    vlines.push_back(l);
    nlines++;
  }
  if (nlines<2)
    return false;
  tx /=nlines;
  ty /=nlines;
  // Offset the lines with the translation
  for (auto & l : vlines)
  {
    double c = l.c();
    c -= l.a()*tx + l.b()*ty;
    l.set(l.a(), l.b(), c);
    tvlines.push_back(l);
  }
  // Scale the lines so that the mean squared distance from the origin is one.
  double c_sq = 0;
  for (auto & l : tvlines)
  {
    c_sq += l.c()*l.c();
  }
  c_sq /=nlines;
  double sigma_c = std::sqrt(c_sq);
  for (auto & l : tvlines)
  {
    double c = l.c();
    if (sigma_c>1e-8)
      c /= sigma_c;
    l.set(l.a(), l.b(), c);
    stvlines.push_back(l);
  }
  // Compute the intersection of the normalized lines to define the vanishing point
  vp = vgl_homg_operators_2d<double>::lines_to_point(stvlines);

  // restore normalizing transform
  // scale factor;
  double lambda = vp.w()/sigma_c;
  vp.set((vp.x()+lambda*tx), (vp.y()+lambda*ty), lambda);

  std::cout << "returning from get_vanishing_point()\n";

  return true;
}

bool sdet_grid_finder::compute_vanishing_points()
{
  if (!groups_valid_)
    return false;
  vgl_homg_point_2d<double> vp0, vp1;
  if (!get_vanishing_point(group0_, vp0))
    return false;
  if (!get_vanishing_point(group1_, vp1))
    return false;
  // if the x component of the first vanishing point is more horizontal then
  // make it the horizontal vanishing point
  if (std::fabs(vp0.x())>std::fabs(vp0.y()))
  {
    vp0_= vp0;
    vp90_= vp1;
  }
  else
  {
    vp0_ = vp1;
    vp90_=vp0;
  }
  vanishing_points_valid_ = true;
  std::cout << "returning from compute_vanishing_points()\n";
  return true;
}

//--------------------------------------------------------------------------
//: find the grid in the set of line segments
bool sdet_grid_finder::match_grid()
{
  return true;
}

//-------------------------------------------------------------------------
//: Compute a projective homography that sends the two major group vanishing points to the x and y directions
bool sdet_grid_finder::compute_projective_homography()
{
  if (projective_homography_valid_)
    return true;

  if (!this->compute_vanishing_points())
  {
    std::cout<< "In sdet_grid_finder::compute_projective_homography() -"
            << " vanishing point computation failed\n";
    return false;
  }
  affine_homography_valid_ = false;
  homography_valid_ = false;
  if (verbose_)
  {
    std::cout << "VP0 " << vp0_ << '\n'
             << "VP90 " << vp90_ << '\n';
  }

  // Keep the sense of the axes pointing to infinity
  vgl_homg_point_2d<double> x_inf(1,0,0), x_minus_inf(-1,0,0);
  vgl_homg_point_2d<double> y_inf(0,1,0), y_minus_inf(0,-1,0);
  vgl_homg_point_2d<double> origin(0,0,1);
  vgl_homg_point_2d<double> x_finite(xmax_,0,1), y_finite(0,ymax_,1);
  vgl_homg_point_2d<double> max_corner(xmax_,ymax_,1);

  // compute a point homography that maps the
  // vanishing points to infinity and leaves the corners of the image invariant
  std::vector<vgl_homg_point_2d<double> > image;
  std::vector<vgl_homg_point_2d<double> > grid;
  // First, if the vanishing points are already quite close to infinity
  // then just form an identity transform, otherwise map to the vanishing pts
  double at_infinity = 1.0e-8;
  if (std::fabs(vp0_.w())<at_infinity)
  {
    image.push_back(x_finite);
    grid.push_back(x_finite);
  }
  else
  {
    image.push_back(vp0_);
    if (vp0_.x()/vp0_.w()>0)
      grid.push_back(x_inf);
    else
      grid.push_back(x_minus_inf);
  }

  if (std::fabs(vp90_.w())<at_infinity)
  {
    image.push_back(y_finite);
    grid.push_back(y_finite);
  }
  else
  {
    image.push_back(vp90_);
    if (vp90_.y()/vp90_.w()>0)
      grid.push_back(y_inf);
    else
      grid.push_back(y_minus_inf);
  }

  image.push_back(origin); image.push_back(max_corner);
  grid.push_back(origin); grid.push_back(max_corner);
  vgl_h_matrix_2d_compute_4point comp_4pt;
  if (!comp_4pt.compute(image, grid, projective_homography_))
    return false;
  if (verbose_)
    std::cout << "The projective homography\n" << projective_homography_ << '\n';
  projective_homography_valid_ = true;
  return true;
}

//---------------------------------------------------------------
//:
//  Remove any remaining skew by finding an affine transformation
//  the maps horizontally inclined lines (angle ah) to zero degrees
//  and vertically inclined lines (angle av) to 90 degrees.
//  That is, find a transformation of the line normals
// \verbatim
//  |q00  q01 ||-sin(av) -sin(ah)|    |-1 0|
//  |q10  q11 || cos(av)  cos(ah)|  = | 0 1|
// \endverbatim
//  The point transformation,Q, is then the inverse transpose of q
// \verbatim
//      | sin(av) -cos(av)|
//  Q = |-sin(ah)  cos(ah)|  = [q]^-t
// \endverbatim
static vnl_matrix_fixed<double, 3,3> skew_transform(const double ah,
                                                    const double av)
{
  vnl_matrix_fixed<double, 3, 3> Q;
  Q.put(0, 0, std::sin(av)); Q.put(0,1, -std::cos(av)); Q.put(0,2,0);
  Q.put(1, 0, -std::sin(ah)); Q.put(1,1, std::cos(ah)); Q.put(1,2,0);
  Q.put(2, 0, 0); Q.put(2,1, 0); Q.put(2,2,1);
  return Q;
}

//------------------------------------------------------------------------
//:Form a histogram of all pairwise distances of lines from the origin.
// h_i is the count, d_i is the average distance in a bin
// The first peak is the distance between grid lines in the horizontal (gh)
// and vertical (gv) directions. The transformation makes each of these
// distances equal to spacing. The result is returned in S.

bool sdet_grid_finder::scale_transform(const double /* max_distance */,
                                       std::vector<vsol_line_2d_sptr> const& gh,
                                       std::vector<vsol_line_2d_sptr> const& gv,
                                       vnl_matrix_fixed<double, 3, 3>& S)
{
  int nbins = 100;
  bsol_distance_histogram Hh(nbins, gh), Hv(nbins, gv);
  double hp1=0, hp2=0, vp1=0, vp2=0;

#undef DEC_DEBUG
#ifdef DEC_DEBUG
  std::cout << "\nHorizontal Distances\n"
           << "*********************************\n";
  for (std::vector<vsol_line_2d_sptr>::const_iterator lit = gh.begin();
       lit != gh.end(); lit++)
  {
    vgl_homg_line_2d<double> hline = (*lit)->vgl_hline_2d();
    hline.normalize();
    std::cout << hline.c() << '\n';
  }
  std::cout << "********************************\n\n"
           << "Vertical Distances\n"
           << "*********************************\n";
  for (std::vector<vsol_line_2d_sptr>::const_iterator lit = gv.begin();
       lit != gv.end(); lit++)
  {
    vgl_homg_line_2d<double> hline = (*lit)->vgl_hline_2d();
    hline.normalize();
    std::cout << hline.c() << '\n';
  }
  std::cout << "********************************\n\n";
#endif
  if (verbose_)
  {
    std::cout << "Horizontal Histogram\n" << Hh << "\n\n"
             << "Vertical Histogram\n" << Hv << "\n\n";
  }
  if (!Hh.distance_peaks(hp1, hp2))
  {
    std::cout << "In sdet_grid_finder::scale_transform(.) - failed"
             << " to find horizontal distance peaks\n";
    return false;
  }
  if (!Hv.distance_peaks(vp1, vp2))
  {
    std::cout << "In sdet_grid_finder::scale_transform(.) - failed"
             << " to find vertical distance peaks\n";
    return false;
  }
  double ph = (hp2-hp1), pv = (vp2-vp1);
  if (verbose_)
  {
    std::cout << "Horizontal Peaks " << hp1 << ' ' << hp2 << '\n'
             << "Vertical Peaks " << vp1 << ' ' << vp2 << '\n'
             << "Horizontal Dist " << ph << '\n'
             << "Vertical Dist " << pv << '\n';
  }
  // failed to find peaks
  if (ph<0||pv<0)
    return false;

  // adjust the spacing to be equal.
  S.put(0, 0, spacing_/pv); S.put(0,1, 0); S.put(0,2,0);
  S.put(1, 0, 0); S.put(1,1, spacing_/ph); S.put(1,2,0);
  S.put(2, 0, 0); S.put(2,1, 0); S.put(2,2,1);
  return true;
}

bool sdet_grid_finder::compute_affine_homography()
{
  if (!projective_homography_valid_)
    return false;
  if (affine_homography_valid_)
    return true;
  float affine_angle_factor = 3.0; // more tolerance for line groups for affine processing

  // transform all the lines using the projective homography defined
  // by vanishing points
  std::vector<vsol_line_2d_sptr> affine_lines;
  for (std::vector<vsol_line_2d_sptr>::const_iterator lit = lines_.begin();
       lit != lines_.end(); lit++)
  {
    vsol_line_2d_sptr pline = this->transform_line(projective_homography_,*lit);
    if (verbose_)
    {
      std::cout << "*lit = ["<<(*lit)->p0()->x()<<','<<(*lit)->p0()->y()<<"] ["
               << (*lit)->p1()->x()<<','<<(*lit)->p1()->y()<<"]\n"
               << "pline = ["<<pline->p0()->x()<<','<<pline->p0()->y()<<"] ["
               << pline->p1()->x()<<','<<pline->p1()->y()<<"]\n";
    }
    // weed out lines with _huge_ coordinates
    if ( (std::abs(pline->p0()->x()) > 10000) ||
         (std::abs(pline->p0()->y()) > 10000) ||
         (std::abs(pline->p1()->x()) > 10000) ||
         (std::abs(pline->p1()->y()) > 10000) )
      continue;
    affine_lines.push_back(pline);
  }

  // Get the bounds of the affine lines (lines with vpoints at infinity)
  vbl_bounding_box<double, 2> b = bsol_algs::bounding_box(affine_lines);
  index_ = new bsol_hough_line_index(b);

  for (auto & affine_line : affine_lines)
    index_->index(affine_line);

  std::vector<std::vector<vsol_line_2d_sptr> > groups;
  if (index_->dominant_line_groups(thresh_,
                                   affine_angle_factor*angle_tol_,
                                   groups)<2)
    return false; // failed to find enough groups.

  afgroup0_ = groups[0];
  afgroup1_ = groups[1];
  if (debug_state_==AFFINE_GROUP_BEFORE_SKEW_SCALE)
  {
    for (std::vector<vsol_line_2d_sptr>::const_iterator lit = afgroup0_.begin();
         lit != afgroup0_.end(); lit++)
      debug_lines_.push_back(*lit);
    for (std::vector<vsol_line_2d_sptr>::const_iterator lit = afgroup1_.begin();
         lit != afgroup1_.end(); lit++)
      debug_lines_.push_back(*lit);
  }

  double avg_angle0=0, avg_angle1=0, min_angle0=0, max_angle0=0,
    min_angle1=0, max_angle1=0;

  group_angle_stats(afgroup0_, angle_tol_,avg_angle0, min_angle0, max_angle0);
  group_angle_stats(afgroup1_, angle_tol_,avg_angle1, min_angle1, max_angle1);

  if (verbose_)
  {
    std::cout << "Affine angles\n"
             << "G[" << afgroup0_.size() << "] avg_angle = " << avg_angle0
             << " min_angle = " << min_angle0 << " max_angle = "
             << max_angle0 << '\n'
             << "G[" << afgroup1_.size() << "] avg_angle = " << avg_angle1
             << " min_angle = " << min_angle1 << " max_angle = "
             << max_angle1 << '\n';
  }
  // Get the orientation of roughly vertical and horizontal lines
  // ang0 is the horizontal direction and ang1 is the vertical direction
  double deg_to_rad = vnl_math::pi_over_180;
  double ang0 =0, ang90=0;
  bool zero_is_zero=true;;
  if (std::fabs(std::fabs(avg_angle0)-90)>std::fabs(std::fabs(avg_angle1)-90))
  {
    ang0 = avg_angle0*deg_to_rad;
    ang90= avg_angle1*deg_to_rad;
    zero_is_zero = true;
  }
  else
  {
    ang0 = avg_angle1*deg_to_rad;
    ang90 = avg_angle0*deg_to_rad;
    zero_is_zero = false;
  }
  // lines should be along the positive x axis.
  if (ang0>vnl_math::pi_over_2)
    ang0-=vnl_math::pi;
  // lines should be along the positive y axis.
  if (ang90<0)
    ang90+=vnl_math::pi;
  // Need to have skew angle at least 60 deg.
  if ((std::fabs(ang90)-std::fabs(ang0))< vnl_math::pi/3.0)
  {
    std::cout << "In sdet_grid_finder::compute_affine_homography() -"
             << " failed to find dominant groups with at least 60 deg"
             << " orientation\n";
    return false;
  }
  vnl_matrix_fixed<double, 3,3> Q = skew_transform(ang0, ang90);

  if (verbose_)
    std::cout << "The skew transform\n" << Q << '\n';

  // max distance for distance histogram
  // the distance could be larger but unlikely
  double dx = std::fabs(b.xmax()-b.xmin()), dy = std::fabs(b.ymax()-b.ymin());
  double max_distance = dx;
  if (dx<dy)
    max_distance = dy;
  vnl_matrix_fixed<double, 3, 3> S;
  if (zero_is_zero)
  {
    if (!scale_transform(max_distance, afgroup0_, afgroup1_, S))
      return false; // failed to find a first distance peak
  }
  else
    if (!scale_transform(max_distance, afgroup1_, afgroup0_, S))
      return false; // failed to find a first distance peak
  if (verbose_)
    std::cout << "The scale transform\n" << S << '\n';

  affine_homography_ = vgl_h_matrix_2d<double>(S*Q);

  // Finally we translate until the first row and column of lines are at (0,0)
#if 0 // Hack! needs to be removed JLM
  double length_threshold = 7.0;
#endif
  std::vector<vsol_line_2d_sptr> grid_lines0, grid_lines90;
  for (auto & lit : afgroup0_)
  {
#if 0 // Hack! needs to be removed JLM
    if ((*lit)->length()<length_threshold)
      continue;
#endif
    if (zero_is_zero)
      grid_lines0.push_back(this->transform_line(affine_homography_,lit));
    else
      grid_lines90.push_back(this->transform_line(affine_homography_,lit));
  }
  for (auto & lit : afgroup1_)
  {
#if 0 // Hack! needs to be removed JLM
    if ((*lit)->length()<length_threshold)
      continue;
#endif
    if (zero_is_zero)
      grid_lines90.push_back(this->transform_line(affine_homography_,lit));
    else
      grid_lines0.push_back(this->transform_line(affine_homography_,lit));
  }
  if ( (debug_state_==AFFINE_GROUP_AFTER_SKEW_SCALE) ||
       (debug_state_==TRANS_PERIM_LINES) )
  {
    for (std::vector<vsol_line_2d_sptr>::const_iterator lit =
         grid_lines0.begin(); lit != grid_lines0.end(); lit++)
      debug_lines_.push_back(*lit);
    for (std::vector<vsol_line_2d_sptr>::const_iterator lit =
         grid_lines90.begin(); lit != grid_lines90.end(); lit++)
      debug_lines_.push_back(*lit);
  }

  std::sort(grid_lines0.begin(), grid_lines0.end(), line_distance);
  std::sort(grid_lines90.begin(), grid_lines90.end(), line_distance);

#ifdef DEBUG
    std::cout <<  "Grid Lines 0\n"
#endif
  if (!chamf0_.insert_lines(grid_lines0,true))
    return false;

#ifdef DEBUG
    std::cout <<  "Grid Lines 90\n"
#endif
  if (!chamf90_.insert_lines(grid_lines90,false))
    return false;

  if (false)
  {
    std::cout << "Grid Lines 0\n";
    print_lines(grid_lines0);
    std::cout << "\n\nGrid Lines 90\n";
    print_lines(grid_lines90);
  }
  for (auto & lit : grid_lines0)
    display_lines_.push_back(lit);

  for (auto & lit : grid_lines90)
    display_lines_.push_back(lit);

  vgl_h_matrix_2d<double> T;
  this->compute_homography_linear_chamfer(T);
  affine_homography_ = T*affine_homography_;
  if (verbose_)
    std::cout << "The affine homography\n" << affine_homography_ << '\n';
  affine_homography_valid_ = true;
  return true;
}


//------------------------------------------------------
//: Assumes that a set of lines have been binned in the 1-d chamfer index
//  Assumes that dindex0_ are vertical lines and dindex90_ are horizontal
bool sdet_grid_finder::
compute_homography_linear_chamfer(vgl_h_matrix_2d<double> & H)
{
  double transx=0, transy=0;

  transx = chamf90_.calculate_grid_offset(n_lines_y_,spacing_);
  transy = chamf0_.calculate_grid_offset(n_lines_x_,spacing_);

  if (verbose_)
    std::cout << " Translation (" << transx << ' ' << transy << ")\n";

  if (debug_state_ == TRANS_PERIM_LINES)
  {
    float xmin = 0, ymin = 0;
    float xmax = 2000, ymax = 2000;
    // vertical lines
    for (int i = 0; i < n_lines_y_; i++)
    {
      vsol_point_2d_sptr p0 = new vsol_point_2d(i*spacing_+transx,ymin);
      vsol_point_2d_sptr p1 = new vsol_point_2d(i*spacing_+transx,ymax);
      vsol_line_2d_sptr line = new vsol_line_2d(p0,p1);
      debug_grid_lines_.push_back(line);
    }
    // horizontal lines
    for (int i = 0; i < n_lines_x_; i++)
    {
      vsol_point_2d_sptr p0 = new vsol_point_2d(xmin,i*spacing_+transy);
      vsol_point_2d_sptr p1 = new vsol_point_2d(xmax,i*spacing_+transy);
      vsol_line_2d_sptr line = new vsol_line_2d(p0,p1);
      debug_grid_lines_.push_back(line);
    }
  }

  // compute new homography so we can double check matched lines
  vnl_matrix_fixed<double, 3, 3> T;
  T.put(0, 0, 1); T.put(0,1, 0); T.put(0,2,-transx);
  T.put(1, 0, 0); T.put(1,1, 1); T.put(1,2,-transy);
  T.put(2, 0, 0); T.put(2,1, 0); T.put(2,2,1);
  vgl_h_matrix_2d<double> Htrans(T);
  double min_x_offset = -10.0, max_x_offset =(n_lines_y_-1)*spacing_ + 10;
  double min_y_offset = -10.0, max_y_offset =(n_lines_x_-1)*spacing_ + 10;
  std::vector<vgl_homg_line_2d<double> > lines_grid, lines_image;
  std::vector<double> weights;
  // insert horizontal line correspondences
  double length_sum = 0;
  for (int i0 = 0; i0< n_lines_x_; i0++)
  {
    double dy = i0*spacing_;
    vgl_homg_line_2d<double> lh(0.0, 1.0, -dy);
    dy += transy;
    std::vector<vsol_line_2d_sptr> h_lines;
    chamf0_.get_lines_in_interval(dy, collection_grid_radius, h_lines);

    if (!h_lines.size())
      continue;
    for (const auto& l0 : h_lines)
    {
      // check x offset
      vsol_line_2d_sptr l0_xformed = this->transform_line(Htrans,l0);
      vsol_point_2d_sptr mid = l0_xformed->middle();
#ifdef DEBUG
      std::cout << "horizontal, p=("<<mid->x()<<','<<mid->y()<<")\n";
#endif
      if ( (mid->x() < min_x_offset) || (mid->x() > max_x_offset) )
      {
#ifdef DEBUG
        std::cout << "discarding line with offset "<<mid->x()<<'\n';
#endif
        continue;
      }
      // line passes: add to set of correspondences
      lines_grid.push_back(lh);
      matched_lines_.push_back(l0);
      if (debug_state_==AFFINE_GROUP_AFTER_TRANS)
        debug_lines_.push_back(l0);
      double length = l0->length();
      length_sum += length;
      weights.push_back(length);
      vgl_homg_line_2d<double> homgl = l0->vgl_hline_2d();
      homgl.normalize();
#ifdef DEBUG
      std::cout << homgl << '\n';
#endif
      lines_image.push_back(homgl);
    }
  }

  for (int i90 = 0; i90< n_lines_y_; i90++)
  {
    double dx = i90*spacing_;
    vgl_homg_line_2d<double> lv(1.0, 0.0, -dx);
    dx += transx;
    std::vector<vsol_line_2d_sptr> v_lines;
    chamf90_.get_lines_in_interval(dx, collection_grid_radius, v_lines);
    if (!v_lines.size())
      continue;
    for (const auto& l90 : v_lines)
    {
      // check y offset
      vsol_line_2d_sptr l90_xformed = this->transform_line(Htrans,l90);
      vsol_point_2d_sptr mid = l90_xformed->middle();
#ifdef DEBUG
      std::cout << "vertical, p=("<<mid->x()<<','<<mid->y()<<")\n";
#endif
      if ( (mid->y() < min_y_offset) || (mid->y() > max_y_offset) )
      {
#ifdef DEBUG
        std::cout << "discarding line with offset "<<mid->y()<<'\n';
#endif
        continue;
      }
      // line passes: add to set of correspondences
      lines_grid.push_back(lv);

      matched_lines_.push_back(l90);
      if (debug_state_==AFFINE_GROUP_AFTER_TRANS)
        debug_lines_.push_back(l90);
      double length = l90->length();
      length_sum += length;
      weights.push_back(length);
      vgl_homg_line_2d<double>  homgl = l90->vgl_hline_2d();
      homgl.normalize();
#ifdef DEBUG
       std::cout << homgl << '\n';
#endif
      lines_image.push_back(homgl);
    }
  }
  if (debug_state_==AFFINE_GROUP_AFTER_TRANS)
  {
    vnl_matrix_fixed<double, 3, 3> T;
    T.put(0, 0, 1); T.put(0,1, 0); T.put(0,2,-transx);
    T.put(1, 0, 0); T.put(1,1, 1); T.put(1,2,-transy);
    T.put(2, 0, 0); T.put(2,1, 0); T.put(2,2,1);
    vgl_h_matrix_2d<double> h(T);
    std::vector<vsol_line_2d_sptr> temp;
    for (auto & debug_line : debug_lines_)
    {
      vsol_line_2d_sptr l = this->transform_line(h,debug_line);
      temp.push_back(l);
    }
    debug_lines_.clear();
    debug_lines_ = temp;
  }

  if (length_sum)
    for (double & weight : weights)
      weight/=length_sum;

  vgl_h_matrix_2d_compute_linear hcl;
#if 0
  double error_term = -1;
#endif
  H = hcl.compute(lines_image, lines_grid, weights /* , error_term */);
  std::cout << "Translation\n" << H << '\n';
#if 0
  std::cout << "Error Term = " << error_term << "\n\n";
#endif

  return true;
}

//: The user will select the four corners of the grid in order to provide
//  a rough estimate of the homography, then grid lines will be used to
//  calculate a fine-tuned homography
bool sdet_grid_finder::compute_manual_homography(const vsol_point_2d_sptr& ul,
                                                 const vsol_point_2d_sptr& ur,
                                                 const vsol_point_2d_sptr& lr,
                                                 const vsol_point_2d_sptr& ll)
{
  // compute initial homography estimate based on manually picked points
  std::vector<vgl_homg_point_2d<double> > image_pts, grid_pts;
  // manually selected image points
  vgl_homg_point_2d<double> im_ul(ul->x(), ul->y());
  vgl_homg_point_2d<double> im_ur(ur->x(), ur->y());
  vgl_homg_point_2d<double> im_lr(lr->x(), lr->y());
  vgl_homg_point_2d<double> im_ll(ll->x(), ll->y());
  image_pts.push_back(im_ul);
  image_pts.push_back(im_ur);
  image_pts.push_back(im_lr);
  image_pts.push_back(im_ll);
  // grid corners
  double min_x = 0, min_y = 0;
  double max_x = (n_lines_x_ - 1) * spacing_;
  double max_y = (n_lines_y_ - 1) * spacing_;
  vgl_homg_point_2d<double> gr_ul(min_x, min_y);
  vgl_homg_point_2d<double> gr_ur(max_x, min_y);
  vgl_homg_point_2d<double> gr_lr(max_x, max_y);
  vgl_homg_point_2d<double> gr_ll(min_x, max_y);
  grid_pts.push_back(gr_ul);
  grid_pts.push_back(gr_ur);
  grid_pts.push_back(gr_lr);
  grid_pts.push_back(gr_ll);
  vgl_h_matrix_2d_compute_linear hcl;
  vgl_h_matrix_2d<double> H = hcl.compute(image_pts, grid_pts);
  std::cout << "initial homography estimate\n" << H << '\n';
  vgl_h_matrix_2d<double> Hinv = H.get_inverse();
  std::cout << "initial homography estimate inverse\n" << Hinv << '\n';

  // associate image lines with grid lines and compute fine-tuned homography
  double bound_x_min = min_x - 5;
  double bound_x_max = max_x + 5;
  double bound_y_min = min_y - 5;
  double bound_y_max = max_y + 5;
  std::vector<vgl_homg_line_2d<double> > grid_lines, image_lines;
  std::vector<double> weights;
  double length_sum = 0;

  for (std::vector<vsol_line_2d_sptr>::const_iterator lit = lines_.begin();
       lit != lines_.end(); lit++)
  {
    // make sure line is correctly defined
    if ( *((*lit)->p0()) == *((*lit)->p1()) )
      continue;

    vgl_homg_line_2d<double> homgl = (*lit)->vgl_hline_2d();
    homgl.normalize();
    // use homography estimate to transform line
    vsol_line_2d_sptr tline = this->transform_line(H,*lit);

#ifdef USE_HTLINE // currently not active
    vgl_homg_line_2d<double> htline = tline->vgl_hline_2d();
    htline.normalize();
#endif
    // weed out lines not in bounding box of grid
    if ( (tline->p0()->x() > bound_x_max) || (tline->p0()->x() < bound_x_min) ||
         (tline->p0()->y() > bound_y_max) || (tline->p0()->y() < bound_y_min) ||
         (tline->p1()->y() > bound_x_max) || (tline->p1()->x() < bound_x_min) ||
         (tline->p1()->y() > bound_y_max) || (tline->p1()->y() < bound_y_min) )
      continue;
    // weed out lines not close to 0 or 90 degrees
    vgl_vector_2d<double> direction = tline->direction();
    // normalize direction vector
    double direction_len = direction.length();
    direction.set(direction.x()/direction_len,
                  direction.y()/direction_len);
    double max_orthog_component = 0.1;

    if (std::fabs(direction.y()) < max_orthog_component)
    {
      // horizontal line
      // find closest grid line
      double length = (*lit)->length();
#ifdef USE_HTLINE // currently not active
      int closest_line = vnl_math::rnd(-htline.c() / spacing_);
#else
      int closest_line = vnl_math::rnd(tline->middle()->y() / spacing_);
#endif
      // TODO: weed out lines not close enough to closest grid line
      vgl_homg_line_2d<double> gl(0.0, 1.0, -spacing_*closest_line);
      grid_lines.push_back(gl);
      length_sum += length;
      weights.push_back(length);
      image_lines.push_back(homgl);
      if (debug_state_==TRANS_PERIM_LINES)
        debug_lines_.push_back(*lit);
      if (debug_state_==AFFINE_GROUP_AFTER_TRANS)
        debug_lines_.push_back(tline);
    }
    else if (std::fabs(direction.x()) < max_orthog_component)
    {
      // vertical line
      // find closest grid line
      double length = (*lit)->length();
      vgl_homg_line_2d<double> homgl = (*lit)->vgl_hline_2d();
      homgl.normalize();
#ifdef USE_HTLINE // currently not active
      int closest_line = vnl_math::rnd(-htline.c() / spacing_);
#else
      int closest_line = vnl_math::rnd(tline->middle()->x() / spacing_);
#endif
      // TODO: weed out lines not close enough to closest grid line
      vgl_homg_line_2d<double> gl(1.0, 0.0, -spacing_*closest_line);
      grid_lines.push_back(gl);
      length_sum += length;
      weights.push_back(length);
      image_lines.push_back(homgl);
      if (debug_state_==TRANS_PERIM_LINES)
        debug_lines_.push_back(*lit);
      if (debug_state_==AFFINE_GROUP_AFTER_TRANS)
        debug_lines_.push_back(tline);
    }
  }
  if (length_sum)
    for (double & weight : weights)
      weight/=length_sum;

#if 0
  double error_term = -1;
#endif
  H = hcl.compute(image_lines, grid_lines, weights /*, error_term */);
  std::cout << "fine tuned homography\n" << H << '\n';
  Hinv = H.get_inverse();
  std::cout << "inverse H =\n" << Hinv << '\n';
#if 0
  std::cout << "Error Term = " << error_term << "\n\n";
#endif

  homography_ = H;

  homography_valid_ = true;

  return true;
}

bool sdet_grid_finder::compute_homography()
{
  static int framenum = 0;
  std::cout << "framenum = "<<framenum++<<'\n';

  if (!this->compute_projective_homography())
  {
    std::cout << "In sdet_grid_finder::compute_homography() -"
             << " projective homography failed\n";
    return false;
  }

  if (!this->compute_affine_homography())
  {
    std::cout << "In sdet_grid_finder::compute_homography() -"
             << " affine homography failed\n";
    return false;
  }

  homography_ = affine_homography_*projective_homography_;

  if (verbose_)
  {
    std::cout << "The composite homography\n" << homography_ << '\n';
  }

  double det = vnl_det(homography_.get_matrix());
  if (det != 0.0)
    homography_valid_ = true;

  return true;
}

//------------------------------------------------------------------------
//: Transform a vsol line using the point transform
//
vsol_line_2d_sptr
sdet_grid_finder::transform_line(vgl_h_matrix_2d<double> const& h,
                                 vsol_line_2d_sptr const & l)
{
  vsol_point_2d_sptr p0 = l->p0();
  vsol_point_2d_sptr p1 = l->p1();
  vgl_homg_point_2d<double> vp0(p0->x(), p0->y());
  vgl_homg_point_2d<double> vp1(p1->x(), p1->y());
  vgl_point_2d<double> tvp0 = h(vp0);
  vgl_point_2d<double> tvp1 = h(vp1);
  vsol_point_2d_sptr tp0 = new vsol_point_2d(tvp0.x(), tvp0.y());
  vsol_point_2d_sptr tp1 = new vsol_point_2d(tvp1.x(), tvp1.y());
  return new vsol_line_2d(tp0, tp1);
}

bool
sdet_grid_finder::get_affine_lines(std::vector<vsol_line_2d_sptr> & lines)
{
  lines = display_lines_;
  return true;
}

bool
sdet_grid_finder::get_grid_points(std::vector<double> &image_x, std::vector<double> &image_y)
{
  if (!homography_valid_)
    return false;

  // grid points are stored in image_x_ and image_y_ here, in
  // anticipation of being used elsewhere by this class

  image_x.clear();
  image_y.clear();
  image_x_.clear();
  image_y_.clear();
  vgl_h_matrix_2d<double> grid_to_image = homography_.get_inverse();

  // x/y mismatch ok - see comment in sdet_grid_finder_params.h
  for (int x = 0; x < n_lines_y_; x++)
  {
    for (int y = 0; y < n_lines_x_; y++)
    {
      vgl_homg_point_2d<double> hp(x*spacing_, y*spacing_);
      vgl_homg_point_2d<double> thp = grid_to_image(hp);
      double imgx = thp.x() / thp.w();
      double imgy = thp.y() / thp.w();
      image_x_.push_back(imgx);
      image_y_.push_back(imgy);
    }
  }
  image_x = image_x_;
  image_y = image_y_;
  return true;
}

bool
sdet_grid_finder::get_debug_lines(std::vector<vsol_line_2d_sptr> & lines)
{
  lines.clear();
  lines = debug_lines_;
  return true;
}

bool
sdet_grid_finder::get_debug_grid_lines(std::vector<vsol_line_2d_sptr> & lines)
{
  lines.clear();
  lines = debug_grid_lines_;
  return true;
}

bool
sdet_grid_finder::get_matched_lines(std::vector<vsol_line_2d_sptr> & lines)
{
  lines = matched_lines_;
  return true;
}

//-------------------------------------------------------------------------
//: Get the original set of lines mapped by the line homography
//
bool
sdet_grid_finder::get_mapped_lines(std::vector<vsol_line_2d_sptr> & lines)
{
  if (!homography_valid_)
    return false;
  lines.clear();
  for (auto & line : lines_)
  {
    vsol_line_2d_sptr l = this->transform_line(homography_,line);
    lines.push_back(l);
  }
  return true;
}


//-------------------------------------------------------------------------
//: Get the grid lines mapped back onto the image
//
bool
sdet_grid_finder::get_backprojected_grid(std::vector<vsol_line_2d_sptr> & lines)
{
  if (!homography_valid_)
    return false;

  lines.clear();
  vgl_h_matrix_2d<double> grid_to_image = homography_.get_inverse();

  // transform the vertical grid lines back to the image
  for (int y = 0; y<n_lines_y_; y++)
  {
    double xv = y*spacing_, maxy = (n_lines_x_-1)*spacing_;
    vgl_homg_point_2d<double> p0(xv,0), p1(xv, maxy);
    vgl_homg_point_2d<double> tp0, tp1;
    tp0 = grid_to_image(p0);  tp1 = grid_to_image(p1);
    vsol_point_2d_sptr sp0 = new vsol_point_2d(tp0);
    vsol_point_2d_sptr sp1 = new vsol_point_2d(tp1);
    vsol_line_2d_sptr lv= new vsol_line_2d(sp0, sp1);
    lines.push_back(lv);
  }
  // transform the horizontal grid lines back to the image
  for (int x = 0; x<n_lines_x_; x++)
  {
    double yv = x*spacing_, maxx = (n_lines_y_-1)*spacing_;
    vgl_homg_point_2d<double> p0(0,yv), p1(maxx, yv);
    vgl_homg_point_2d<double> tp0, tp1;
    tp0 = grid_to_image(p0);  tp1 = grid_to_image(p1);
    vsol_point_2d_sptr sp0 = new vsol_point_2d(tp0);
    vsol_point_2d_sptr sp1 = new vsol_point_2d(tp1);
    vsol_line_2d_sptr lv = new vsol_line_2d(sp0, sp1);
    lines.push_back(lv);
  }
#ifdef DEBUG
  // temp for kongbin -DEC
   write_image_points(grid_to_image);
#endif

  return true;
}

//: initialize and output file. The file will be in the format taken by
//  the zhang calibration routine located in bmvl/bcal
bool sdet_grid_finder::init_output_file(std::ofstream & outstream)
{
  outstream << (n_lines_x_*n_lines_y_) << '\n';
  // x/y mismatch ok - see comment in sdet_grid_finder_params.h
  for (int x = 0; x < n_lines_y_; x++)
  {
    for (int y = 0; y < n_lines_x_; y++)
    {
      outstream << x*spacing_<<' '<<y*spacing_<<'\n';
    }
  }
  outstream << "NUM_VIEWS_PLACEHOLDER\n";
  return true;
}

//: append the output file with points calculated using homography_
bool sdet_grid_finder::write_image_points(std::ofstream & outstream)
{
  vgl_h_matrix_2d<double> grid_to_image = homography_.get_inverse();

  // x/y mismatch ok - see comment in sdet_grid_finder_params.h
  for (int x=0; x<n_lines_y_; x++)
  {
    for (int y=0; y<n_lines_x_; y++)
    {
      vgl_homg_point_2d<double> hp(x*spacing_, y*spacing_);
      vgl_homg_point_2d<double> thp = grid_to_image(hp);
      double image_x = thp.x() / thp.w();
      double image_y = thp.y() / thp.w();
      outstream << image_x <<' '<<image_y<<'\n';
    }
  }
  outstream << '\n';

  return true;
}


bool sdet_grid_finder::transform_grid_points(vnl_matrix_fixed<double,3,3> & K,
                                             vnl_matrix_fixed<double,3,4> & M,
                                             std::vector<vsol_point_2d_sptr> & points)
{
  vnl_matrix_fixed<double,3,4> grid_to_image_debug = K*M;
  // transform the vertical grid lines back to the image
  for (int x = 0; x<n_lines_x_; x++)
  {
    for (int y = 0; y<n_lines_y_; y++)
    {
      vnl_matrix_fixed<double,4,1> p;
      p.put(0,0,x*spacing_); p.put(1,0,y*spacing_); p.put(2,0,0); p.put(3,0,1);

      vnl_matrix_fixed<double,3,1> tp = grid_to_image_debug * p;

      vsol_point_2d_sptr sp = new vsol_point_2d(tp.get(0,0)/tp.get(2,0),
                                                tp.get(1,0)/tp.get(2,0));
      points.push_back(sp);
    }
  }
  return true;
}

//----------------------------------------------------------
//: Clear internal storage
//
void sdet_grid_finder::clear()
{
  lines_.clear();
  vanishing_points_valid_=false;
  projective_homography_valid_ = false;
  homography_valid_ = false;
}


//: Check grid match by verifying image intensity values within grid squares
bool sdet_grid_finder::check_grid_match(const vil1_image& img)
{
  if (!homography_valid_)
    return false;

  double mean_intensity, intensity_sigma;
  // we want to make sure all of the squares in one category (the white squares)
  // have intensity greater than all the squares in the other category (black squares)
  double square_max_mins[2][2];
  // category 1 min
  square_max_mins[0][0] = vnl_numeric_traits<double>::maxval;
  // category 1 max
  square_max_mins[0][1] = 0;
  // category 2 min
  square_max_mins[1][0] = vnl_numeric_traits<double>::maxval;
  // category 2 max
  square_max_mins[1][1] = 0;

  int category;
  // check left row of grid squares
  for (int i = 0; i < n_lines_x_-1; i++)
  {
    if (!get_square_pixel_stats(img,0,i,mean_intensity,intensity_sigma))
      return false;
    category = i%2;
    if (mean_intensity < square_max_mins[category][0])
      square_max_mins[category][0] = mean_intensity;
    if (mean_intensity > square_max_mins[category][1])
      square_max_mins[category][1] = mean_intensity;
  }
  // check right row of grid squares
  for (int i = 0; i < n_lines_x_-1; i++)
  {
    if (!get_square_pixel_stats(img,n_lines_y_-2,i,mean_intensity,intensity_sigma))
      return false;
    category = int(std::abs(double((n_lines_y_%2) - (i%2))));
    if (mean_intensity < square_max_mins[category][0])
      square_max_mins[category][0] = mean_intensity;
    if (mean_intensity > square_max_mins[category][1])
      square_max_mins[category][1] = mean_intensity;
  }
  // check upper row of grid squares
  for (int i = 0; i < n_lines_y_-1; i++)
  {
    if (!get_square_pixel_stats(img,i,0,mean_intensity,intensity_sigma))
      return false;
    category = (i%2);
    if (mean_intensity < square_max_mins[category][0])
      square_max_mins[category][0] = mean_intensity;
    if (mean_intensity > square_max_mins[category][1])
      square_max_mins[category][1] = mean_intensity;
  }
  // check lower row of grid squares
  for (int i = 0; i < n_lines_y_-1; i++)
  {
    if (!get_square_pixel_stats(img,i,n_lines_x_-2,mean_intensity,intensity_sigma))
      return false;
    category = int(std::abs(double((n_lines_x_%2) - (i%2))));
    if (mean_intensity < square_max_mins[category][0])
      square_max_mins[category][0] = mean_intensity;
    if (mean_intensity > square_max_mins[category][1])
      square_max_mins[category][1] = mean_intensity;
  }
  std::cout << "cat 1: min = "<<square_max_mins[0][0]<<" max = "<<square_max_mins[0][1]<<'\n'
           << "cat 2: min = "<<square_max_mins[1][0]<<" max = "<<square_max_mins[1][1]<<'\n';

  if ( (square_max_mins[0][0] > square_max_mins[1][1]) ||
       (square_max_mins[1][0] > square_max_mins[0][1]) )
  {
    return true;
  }
  else
  {
    std::cout << "INVALID GRID MATCH\n";
    return false;
  }
}

//: gets pixels stats from img within grid square specified by x,y
//  Not returning a valid sigma value for now -DEC
bool sdet_grid_finder::get_square_pixel_stats(const vil1_image& img,
                                              int x,int y,
                                              double & mean_intensity,
                                              double & /* intensity_sigma */)
{
  gevd_bufferxy img_buff(img);

  vgl_homg_point_2d<double> gul(x*spacing_,y*spacing_);
  vgl_homg_point_2d<double> gur((x+1)*spacing_,y*spacing_);
  vgl_homg_point_2d<double> glr((x+1)*spacing_,(y+1)*spacing_);
  vgl_homg_point_2d<double> gll(x*spacing_,(y+1)*spacing_);
  vgl_homg_point_2d<double> ul,ur,lr,ll;

  vgl_h_matrix_2d<double> grid_to_image = homography_.get_inverse();

  ul = grid_to_image(gul); ul.set(ul.x()/ul.w(), ul.y()/ul.w(),1.0);
  ur = grid_to_image(gur); ur.set(ur.x()/ur.w(), ur.y()/ur.w(),1.0);
  lr = grid_to_image(glr); lr.set(lr.x()/lr.w(), lr.y()/lr.w(),1.0);
  ll = grid_to_image(gll); ll.set(ll.x()/ll.w(), ll.y()/ll.w(),1.0);

#ifdef DEBUG
  std::cout << "ul "<<ul<<" ur "<<ur<<" lr "<<lr<<" ll "<<ll<<'\n';
#endif
  // add/subtract 1 from min_y and max_y to account for possible rounding error
  int min_y =
    vnl_math::rnd(std::min(std::min(std::min(ul.y(),ur.y()),ll.y()),lr.y())) -1;
  int max_y =
    vnl_math::rnd(std::max(std::max(std::max(ul.y(),ur.y()),ll.y()),lr.y())) +1;

  int n_scan_rows = max_y - min_y + 1;

  if (n_scan_rows < 2*n_lines_x_)
  {
    // less than 2 pixels per square - something 's wrong
    return false;
  }

  vnl_matrix<int> scan_rows(n_scan_rows,2);

  for (int i = 0; i < n_scan_rows; i++)
  {
    scan_rows[i][0] = vnl_numeric_traits<int>::maxval;
    scan_rows[i][1] = -vnl_numeric_traits<int>::maxval;
  }

  // start_t and end_t are set so we stay within the square region,
  // avoiding the boundary pixels

  // upper line
  int start_t = vnl_math::rnd(ul.x()+0.5);
  int end_t = vnl_math::rnd(ur.x()-0.5);
  double slope = (ur.y() - ul.y())/(ur.x() - ul.x());
  double real_pix = ul.y();
  int rounded_pix;
  int row_idx;
#ifdef DEBUG
  std::cout << "start_t = "<<start_t<<" end_t= "<<end_t<<" slope = "<<slope<<'\n';
#endif
  for (int t = start_t; t <= end_t; t++)
  {
    rounded_pix = vnl_math::rnd(real_pix);
    row_idx = rounded_pix - min_y;
    scan_rows[row_idx][0] = std::min(scan_rows[row_idx][0],t);
    scan_rows[row_idx][1] = std::max(scan_rows[row_idx][1],t);
    real_pix += slope;
  }

  // lower line
  start_t = vnl_math::rnd(ll.x()+0.5);
  end_t = vnl_math::rnd(lr.x()-0.5);
  slope = (lr.y() - ll.y())/(lr.x() - ll.x());
  real_pix = ll.y();
#ifdef DEBUG
  std::cout << "start_t = "<<start_t<<" end_t= "<<end_t<<" slope = "<<slope<<'\n';
#endif
  for (int t = start_t; t <= end_t; t++)
  {
    rounded_pix = vnl_math::rnd(real_pix);
    row_idx = rounded_pix - min_y;
    scan_rows[row_idx][0] = std::min(scan_rows[row_idx][0],t);
    scan_rows[row_idx][1] = std::max(scan_rows[row_idx][1],t);
    real_pix += slope;
  }

  // left line
  start_t = vnl_math::rnd(ul.y()+0.5);
  end_t = vnl_math::rnd(ll.y()-0.5);
  slope = (ll.x() - ul.x())/(ll.y() - ul.y());
  real_pix = ul.x();
#ifdef DEBUG
  std::cout << "start_t = "<<start_t<<" end_t= "<<end_t<<" slope = "<<slope<<'\n';
#endif
  for (int t = start_t; t <= end_t; t++)
  {
    rounded_pix = vnl_math::rnd(real_pix);
    row_idx = t - min_y;
    scan_rows[row_idx][0] = std::min(scan_rows[row_idx][0],rounded_pix);
    scan_rows[row_idx][1] = std::max(scan_rows[row_idx][1],rounded_pix);
    real_pix += slope;
  }

  // right line
  start_t = vnl_math::rnd(ur.y()+0.5);
  end_t = vnl_math::rnd(lr.y()-0.5);
  slope = (lr.x() - ur.x())/(lr.y() - ur.y());
  real_pix = ur.x();
#ifdef DEBUG
  std::cout << "start_t = "<<start_t<<" end_t= "<<end_t<<" slope = "<<slope<<'\n';
#endif
  for (int t = start_t; t <= end_t; t++)
  {
    rounded_pix = vnl_math::rnd(real_pix);
    row_idx = t - min_y;
    scan_rows[row_idx][0] = std::min(scan_rows[row_idx][0],rounded_pix);
    scan_rows[row_idx][1] = std::max(scan_rows[row_idx][1],rounded_pix);
    real_pix += slope;
  }
  // now we can scan the pixels and grab their intensities
  double intensity_total = 0.0;
  int n_pix = 0;
  for (int i = 0; i < n_scan_rows; i++)
  {
    for (int j = scan_rows[i][0]; j <= scan_rows[i][1]; ++j)
    {
      int xx = j, yy = i + min_y;
      if (xx < 0 || xx >= img_buff.GetSizeX())
        continue;
      if (yy < 0 || yy >= img_buff.GetSizeY())
        continue;
      intensity_total+= *((unsigned char*)(img_buff.GetElementAddr(xx,yy)));
      ++n_pix;
    }
  }
  if (!n_pix)
  {
    std::cout << "grid square contains 0 pixels\n";
    return false;
  }
  mean_intensity =  intensity_total / n_pix;
  if (verbose_)
    std::cout << '('<<x<<','<<y<<") mean_intensity = "<<mean_intensity<<'\n';
  return true;
}
