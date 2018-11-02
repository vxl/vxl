#include <cmath>
#include <iostream>
#include <algorithm>
#include "bsol_hough_line_index.h"
//:
// \file
//
// Modifications : see bsol_hough_line_index.h
//
//-----------------------------------------------------------------------------


#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#undef DEGTORAD
#define DEGTORAD 0.017453293f // = float(vnl_math::pi_over_180)

class nlines
{
 public:
  bool operator()(const std::vector<vsol_line_2d_sptr> & v1,
                  const std::vector<vsol_line_2d_sptr> & v2)
  {
    return v1.size() > v2.size();
  }
};

//--------------------------------------------------------------
//
// Constructor and Destructor Functions
//
//--------------------------------------------------------------

//---------------------------------------------------------------
//: Simple Constructor
//
bsol_hough_line_index::bsol_hough_line_index(const int r_dimension,
                                             const int theta_dimension)
{
  xo_ = 0; yo_ = 0;
  ysize_ = xsize_ = (float)std::ceil(r_dimension/vnl_math::sqrt2);
  angle_range_ = (float)theta_dimension;
  angle_increment_ = 1.0f;

  this->init(r_dimension, theta_dimension);
}

//--------------------------------------------------------------
//: Constructors from given bounds
//
bsol_hough_line_index::bsol_hough_line_index(const float x0,
                                             const float y0,
                                             const float xsize,
                                             const float ysize,
                                             const float angle_range,
                                             const float angle_increment)
{
  xo_ = x0;  yo_ = y0;
  xsize_ = xsize;
  ysize_ = ysize;
  angle_range_ = angle_range;
  angle_increment_ = angle_increment;

  int theta_dimension = (int)std::ceil(angle_range_/angle_increment_);
  theta_dimension++; //Include both 0 and angle_range_
  float diag = std::sqrt(xsize*xsize + ysize*ysize);
  int rmax = int(diag);
  rmax++; //Round off.
  this->init(rmax, theta_dimension);
}

bsol_hough_line_index::
bsol_hough_line_index(vbl_bounding_box<double, 2> const& box,
                      const float angle_range,
                      const float angle_increment)
{
  xo_ = float(box.xmin());  yo_ = float(box.ymin());
  xsize_ = float(box.xmax()-xo_);
  ysize_ = float(box.ymax()-yo_);
  angle_range_ = angle_range;
  angle_increment_ = angle_increment;

  int theta_dimension = (int)std::ceil(angle_range_/angle_increment_);
  theta_dimension++; //Include both 0 and angle_range_
  float diag = std::sqrt(xsize_*xsize_ + ysize_*ysize_);
  int rmax = int(diag);
  rmax++; //Round off.
  this->init(rmax, theta_dimension);
}

//: Destructor
bsol_hough_line_index::~bsol_hough_line_index()
{
  for (int r=0;r<r_dim_;r++)
    for (int th=0;th<th_dim_;th++)
      delete index_[r][th];
}

//-----------------------------------------------------------------------------
//
//: Compute the bsol_hough_line_index array locations corresponding to a line
//
void bsol_hough_line_index::array_loc(vsol_line_2d_sptr const& line,
                                      float& r, float& theta)
{
  //Compute angle index
  auto angle = (float)line->tangent_angle();
  if (angle >= 180.0f)
    angle -= 180.0f;

  if (angle > angle_range_)
  {
    std::cout << "Warning - bsol_hough_line_index angle outside of range!\n";
    return;
  }

  theta = angle;

  float angrad = DEGTORAD*angle;

  //Compute distance indices
  vsol_point_2d_sptr mid = line->middle();
  auto midx = float(mid->x()-xo_);
  auto midy = float(mid->y()-yo_);
  float xs2 = xsize_/2.0f;
  float ys2 = ysize_/2.0f;

  double cx = -(midx-xs2)*std::sin(angrad);
  double cy =  (midy-ys2)*std::cos(angrad);

  //We use the middle of the ranges as the origin to insure minimum error
  //Also, the distance index is guaranteed to be positive
  r = float(cx + cy + std::sqrt(xs2*xs2 + ys2*ys2));
}

//-----------------------------------------------------------------------------
//
//: Compute the bsol_hough_line_index array locations corresponding to a line
//
void bsol_hough_line_index::array_loc(vsol_line_2d_sptr const& line,
                                      int& r, int& theta)
{
  float angle = 0, radius = 0;
  this->array_loc(line, radius, angle);
  theta = (int)std::floor(angle/angle_increment_);
  r = int(radius);
}

//-----------------------------------------------------------------------------
//
//: Modify bsol_hough_line_index array R location under translation
//
int bsol_hough_line_index::trans_loc(const int transx, const int transy,
                                     const int r, const int theta)
{
  float angle = angle_increment_*theta;
  float angrad = DEGTORAD*angle;
  int new_cx = -int(transx*std::sin(angrad));
  int new_cy =  int(transy*std::cos(angrad));
  int newr = new_cx + new_cy;
  newr += r;
  if (newr < 0)
    return 0;
  else if (newr > r_dim_)
    return r_dim_;
  else
    return newr;
}

//-----------------------------------------------------------------------------
//
//: Return the count at a given r and theta
//
//-----------------------------------------------------------------------------
int bsol_hough_line_index::count(const int r, const int theta)
{
  if (r<0||theta<0||r>=r_dim_||theta>=th_dim_)
  {
    std::cout << "Warning - bsol_hough_line_index index outside of range!\n";
    return 0;
  }

  return index_[r][theta]->size();
}

//-----------------------------------------------------------------------------
//
//: Method to index into bsol_hough_line_index index given a vsol_line_2d_sptr.
//  The timestamp is not updated to facilitate lazy insertion of
//  multiple items.
bool bsol_hough_line_index::index(vsol_line_2d_sptr const& line)
{
  if (!line)
  {
    std::cout << "In bsol_hough_line_index::index(..) NULL line\n";
    return false;
  }
  int r, theta;
  this->array_loc(line, r, theta);
  if (!(r < r_dim_)||!(theta < th_dim_))
    return false;

  index_[r][theta]->push_back(line);
  return true;
}

//-----------------------------------------------------------------------------
//
//: Method to index into bsol_hough_line_index index given a vsol_line_2d_sptr.
//  Only new vsol_line_2d_sptr pointers are added to the bin.
bool bsol_hough_line_index::index_new(vsol_line_2d_sptr const& line)
{
  if (!line)
  {
    std::cout << "In bsol_hough_line_index::index_new(..) NULL line\n";
    return false;
  }

  int r, theta;
  //Check array bounds and uniqueness of line
  this->array_loc(line, r, theta);
  if (!(r < r_dim_)||!(theta < th_dim_))
    return false;

  std::vector<vsol_line_2d_sptr>* lines = index_[r][theta];
  if (!(std::find(lines->begin(), lines->end(), line) == lines->end()))
    return false;

  index_[r][theta]->push_back(line);
  return true;
}

//-----------------------------------------------------------------------------
//
//: find if a line is in the array
bool bsol_hough_line_index::find(vsol_line_2d_sptr const& line)
{
  int r, theta;
  this->array_loc(line, r, theta);
  std::vector<vsol_line_2d_sptr>* lines = index_[r][theta];
  return !(std::find(lines->begin(), lines->end(), line) == lines->end());
}

//-----------------------------------------------------------------------------
//
//: remove a line from the Hough index
//  The timestamp is not updated to facilitate lazy insertion of
//  multiple items.  See vsol_line_2d_sptrGroup as an example.
bool bsol_hough_line_index::remove(vsol_line_2d_sptr const& line)
{
  int r, theta;
  this->array_loc(line, r, theta);
  if (!(r < r_dim_)||!(theta < th_dim_))
    return false;
  std::vector<vsol_line_2d_sptr>* lines = index_[r][theta];

  auto lit =
    std::find(lines->begin(), lines->end(), line);
  if (lit == lines->end())
    return false;
  lines->erase(lit);
  return true;
}

//-----------------------------------------------------------------------------
//
//: Fill a vector of vsol_line_2d_sptr(s) which are at the index location
//
//-----------------------------------------------------------------------------
void
bsol_hough_line_index::lines_at_index(const int r, const int theta,
                                      std::vector<vsol_line_2d_sptr>& lines)
{
  lines.clear();
  if ((theta<0)||(theta>=th_dim_)||(r<0)||(r>=r_dim_))
    return;

  int count = this->count(r, theta);
  if (count==0)
    return;

  for (int i = 0; i<count; i++)
    lines.push_back((*index_[r][theta])[i]);
}

//-----------------------------------------------------------------------------
//
//: Return a list of vsol_line_2d_sptr(s) which are at the index location
//
//-----------------------------------------------------------------------------
std::vector<vsol_line_2d_sptr> bsol_hough_line_index::
lines_at_index(const int r, const int theta)
{
  std::vector<vsol_line_2d_sptr> out;
  this->lines_at_index(r, theta, out);
  return out;
}

//-----------------------------------------------------------------------------
//
//: Fill a list of vsol_line_2d_sptr(s) which are within a distance(radius) of the r,theta values of a given line.
//
//-----------------------------------------------------------------------------

void bsol_hough_line_index::
lines_in_interval(vsol_line_2d_sptr const & l,
                  const float r_dist,
                  const float theta_dist,
                  std::vector<vsol_line_2d_sptr>& lines)
{
  lines.clear();
  if (!l)
  {
    std::cout << "In bsol_hough_line_index::lines_in_interval(..) NULL line\n";
    return;
  }
  int r, theta;
  this->array_loc(l, r, theta);
  int angle_radius = (int)std::ceil(theta_dist/angle_increment_);
  int r_radius = (int)std::ceil(r_dist);
  int th_dim_m1 = th_dim_ - 1;

  for (int i = -angle_radius; i<=angle_radius; i++)
  {
    //The angle space is circular
    int t_indx = (theta + i) % (th_dim_m1);
    if (t_indx<0)
      t_indx += th_dim_m1;

    for (int j = -r_radius; j<=r_radius; j++)
    {
      int r_indx = r + j;
      if ((r_indx<0)||(r_indx>=r_dim_))
        continue;
      std::vector<vsol_line_2d_sptr> temp;
      this->lines_at_index(r_indx, t_indx,temp);
      for (auto line : temp)
      {
        //Note, these tests should eventually be more
        //sophisticated - JLM
        float l_angle, line_angle;
        float l_ndist, line_ndist;
        this->array_loc(l, l_ndist, l_angle);
        this->array_loc(line, line_ndist, line_angle);

        //Test error in normal distance
        bool within_r_radius = std::fabs(l_ndist - line_ndist) < r_dist;
        if (!within_r_radius)
          continue;

        //Test angular error
        bool within_angle_radius = std::fabs(l_angle - line_angle) < theta_dist;
        if (!within_angle_radius)
          continue;

        //line, passed both tests
        lines.push_back(line);
      }
    }
  }
}

//-----------------------------------------------------------------------------
//
//: Return a list of vsol_line_2d_sptr(s) which are within a distance(radius) of the r,theta values of a given line.
//
//-----------------------------------------------------------------------------

std::vector<vsol_line_2d_sptr>
bsol_hough_line_index::lines_in_interval(vsol_line_2d_sptr const & l,
                                         const float r_dist,
                                         const float theta_dist)
{
  std::vector<vsol_line_2d_sptr> out;
  if (!l)
  {
    std::cout << "In bsol_hough_line_index::lines_in_interval(..) NULL line\n";
    return out;
  }

  this->lines_in_interval(l, r_dist, theta_dist, out);
  return out;
}

//-----------------------------------------------------------------------------
//
//: Fill a list of vsol_line_2d_sptr(s) which are within angle_dist of a given angle
//
//-----------------------------------------------------------------------------

void
bsol_hough_line_index::parallel_lines(const float angle,
                                      const float angle_dist,
                                      std::vector<vsol_line_2d_sptr>& lines)
{
  lines.clear();

  //Compute angle index and tolerance
  float ang = std::fmod(angle,180.f);
  if (ang < 0) ang = 180.0f-ang;

  if (ang > angle_range_)
  {
    std::cout << "Warning - bsol_hough_line_index angle outside of range!\n";
    return;
  }
  int theta = (int)std::floor(ang/angle_increment_);
  int angle_radius = (int)std::ceil(angle_dist/angle_increment_);
  int th_dim_m1 = th_dim_ - 1;

  for (int i = -angle_radius; i<=angle_radius; i++)
  {
    //The angle space is circular
    int t_indx = (theta + i) % (th_dim_m1);
    if (t_indx<0)
      t_indx += th_dim_m1;
    for (int j = 0; j<r_dim_; j++)
    {
      if (!(this->count(j, t_indx)>0))
        continue;
      std::vector<vsol_line_2d_sptr> temp;
      this->lines_at_index(j, t_indx, temp);

      for (auto line : temp)
      {
        //Test angular error
        auto line_angle = (float)line->tangent_angle();
        if (line_angle >= 180.0f)
          line_angle -= 180.0f;
        float ang_error = std::fabs(ang - line_angle);
        if (ang_error<angle_dist)
          lines.push_back(line);
        ang_error-=180.0f;//anti-parallel lines are included
        if (std::fabs(ang_error)<angle_dist)
          lines.push_back(line);
      }
    }
  }
}

//-----------------------------------------------------------------------------
//
//: Return a list of vsol_line_2d_sptr(s) which are within angle_dist of a given angle
//
//-----------------------------------------------------------------------------

std::vector<vsol_line_2d_sptr >
bsol_hough_line_index::parallel_lines(const float angle,
                                      const float angle_dist)
{
  std::vector<vsol_line_2d_sptr> out;
  this->parallel_lines(angle, angle_dist, out);
  return out;
}

//-----------------------------------------------------------------------------
//
//: Fill a list of vsol_line_2d_sptr(s) which are oriented at an angle with respect to a given line.
//  A specified angular tolerance can be given.
//
//-----------------------------------------------------------------------------

void
bsol_hough_line_index::lines_at_angle(vsol_line_2d_sptr const &l,
                                      const float angle,
                                      const float angle_dist,
                                      std::vector<vsol_line_2d_sptr >& lines)
{
  lines.clear();
  if (!l)
  {
    std::cout << "In bsol_hough_line_index::lines_at_angle(..) NULL line\n";
    return;
  }
  float ang = (float)l->tangent_angle() + angle;
  this->parallel_lines(ang, angle_dist, lines);
}

//-----------------------------------------------------------------------------
//
//: Return a new list of vsol_line_2d_sptr(s) which are oriented at an angle with respect to a given line.
//  A specified angular tolerance can be given.
//-----------------------------------------------------------------------------

std::vector<vsol_line_2d_sptr>
bsol_hough_line_index::lines_at_angle(vsol_line_2d_sptr const &l,
                                      const float angle,
                                      const float angle_dist)
{
  std::vector<vsol_line_2d_sptr> out;
  this->lines_at_angle(l, angle, angle_dist, out);
  return out;
}

//-----------------------------------------------------------------------------
//
//: Fill a list of vsol_line_2d_sptr(s) which are within angle_dist of the orientation of a given line.
//
//-----------------------------------------------------------------------------

void
bsol_hough_line_index::parallel_lines(vsol_line_2d_sptr const &l,
                                      const float angle_dist,
                                      std::vector<vsol_line_2d_sptr>& lines)
{
  lines.clear();
  if (!l)
  {
    std::cout << "In bsol_hough_line_index::parallel_lines(..) NULL line\n";
    return;
  }
  auto angle = (float)l->tangent_angle();
  this->parallel_lines(angle, angle_dist, lines);
}

//-----------------------------------------------------------------------------
//
//: Return a list of vsol_line_2d_sptr(s) which are within angle_dist of the orientation of a given line.
//
//-----------------------------------------------------------------------------

std::vector<vsol_line_2d_sptr>
bsol_hough_line_index::parallel_lines(vsol_line_2d_sptr const &l,
                                      const float angle_dist)
{
  std::vector<vsol_line_2d_sptr> out;
  if (!l)
  {
    std::cout << "In bsol_hough_line_index::parallel_lines(..) NULL line\n";
    return out;
  }
  this->parallel_lines(l, angle_dist, out);
  return out;
}

//-----------------------------------------------------------------------------
//
//: Clear the bsol_hough_line_index index space
//
//-----------------------------------------------------------------------------
void bsol_hough_line_index::clear_index()
{
  for (int r=0;r<r_dim_;r++)
    for (int th=0;th<th_dim_;th++)
      index_[r][th]->clear();
}

//-----------------------------------------------------------------
//: Constructor Utility
//

void bsol_hough_line_index::init(const int r_dimension,
                                 const int theta_dimension)
{
  r_dim_ = r_dimension;
  th_dim_ = theta_dimension;
  index_.resize(r_dim_, th_dim_);
  for (int r=0;r<r_dim_;r++)
    for (int th=0;th<th_dim_;th++)
      index_.put(r,th, new std::vector<vsol_line_2d_sptr>);
}

//-----------------------------------------------------------------
//: Fill the angle histogram
//
std::vector<int> bsol_hough_line_index::angle_histogram()
{
  std::vector<int> angle_hist(th_dim_);
  for (int x = 0; x<th_dim_; x++)
  {
    angle_hist[x]=0;
    for (int y = 0; y<r_dim_; y++)
    {
      std::vector<vsol_line_2d_sptr>* lines = index_[y][x];
      if (lines)
        angle_hist[x]+=lines->size();
    }
  }
  return angle_hist;
}

//-------------------------------------------------------------------
//:
// Provides the correct values for angle histogram counts when the bin index
// extends outside the valid range of the counts array.  This function
// permits easy access logic for the non_maximum_suppression algorithm.
static int get_extended_count(int bin, std::vector<int> const& ang_hist)
{
  int n_bins = ang_hist.size();
  int nbm = n_bins-1;
  //Abnormal Cases
  if (bin<0)
    return ang_hist[nbm+bin];

  if (bin >= n_bins)
    return ang_hist[bin-n_bins];
  //Normal Case
  return ang_hist[bin];
}

//---------------------------------------------------------------------
//: Prune any sequences of more than one maximum value.
// That is, it is possible to have a "flat" top peak with an arbitrarily
// long sequence of equal, but maximum values.
static void remove_flat_peaks(std::vector<int>& angle_hist)
{
  int nbins = angle_hist.size();
  int nbm = nbins-1;

  //Here we define a small state machine - parsing for runs of peaks
  //init is the state corresponding to an initial run (starting at i ==0)
  bool init= get_extended_count(0, angle_hist)>0;
  int init_end =0;

  //start is the state corresponding to any other run of peaks
  bool start=false;
  int start_index=0;

  //The scan of the state machine
  for (int i = 0; i < nbins; i++)
  {
    int v = get_extended_count(i, angle_hist);

    //State init: a string of non-zeroes at the beginning.
    if (init && v!=0)
      continue;

    if (init && v==0)
    {
      init_end = i;
      init = false;
      continue;
    }

    //State !init&&!start: a string of "0s"
    if (!start && v==0)
      continue;

    //State !init&&start: the first non-zero value
    if (!start && v!=0)
    {
      start_index = i;
      start = true;
      continue;
    }
    //State ending flat peak: encountered a subsequent zero after starting
    if (start && v==0)
    {
      int peak_location = (start_index+i-1)/2;//The middle of the run
      for (int k = start_index; k<=(i-1); k++)
        if (k!=peak_location)
          angle_hist[k] = 0;
      start = false;
    }
  }
  //Now handle the boundary conditions
  if (init_end!=0) { //Is there a run which crosses the cyclic cut?
    if (start)
    {    //Yes, so define the peak location accordingly
      int peak_location = (start_index + init_end - nbm -1)/2;
      int k;
      if (peak_location < 0) //Is the peak to the left of the cut?
      {   // Yes, to the left
        peak_location += nbm;
        for ( k = 0; k< init_end; k++)
          angle_hist[k]=0;
        for ( k= start_index; k <nbins; k++)
          if (k!=peak_location)
            angle_hist[k] = 0;
      }
      else
      {   //No, on the right.
        for ( k = start_index; k< nbins; k++)
          angle_hist[k]=0;
        for ( k= 0; k < init_end; k++)
          if (k!=peak_location)
            angle_hist[k] = 0;
      }
    }
    else
    {   //There wasn't a final run so just clean up the initial run
      int init_location = (init_end-1)/2;
      for (int k = 0; k<=init_end; k++)
        if (k!=init_location)
          angle_hist[k] = 0;
    }
  }
}

//----------------------------------------------------------
//: Suppress values in the angle histogram which are not locally a maximum.
//  The neighborhood for computing the local maximum
//  is [radius X radius], e.g. for radius =1 the neighborhood
//  is [-X-], for radius = 2, the neighborhood is [--X--], etc.
//
std::vector<int>
bsol_hough_line_index::non_maximum_suppress(const int radius,
                                            std::vector<int> const& bins)
{
  int num = bins.size();
  std::vector<int> out(num);
  if (4*radius +2 > num)
  {
    std::cout << "bsol_hough_line_index::non_maximum_suppress(..) - radius is too large\n";
    return out;
  }

  //Clear the output
  for (int indx =0; indx < num; indx++)
    out[indx] = 0;

  //Find local maxima
  for (int indx = 0; indx<num; indx++)
  {
    //find the maximum value in the current kernel
    int max_count = bins[indx];
    for (int k = -radius; k <= radius ;k++)
    {
      int index = indx+k;
      int c = get_extended_count(index, bins);
      if ( c > max_count)
        max_count = c;
    }
    //Is position th a local maximum?
    if (max_count == bins[indx])
      out[indx] = max_count;//Yes. So set the counts to the max value
  }
  remove_flat_peaks(out);
  return out;
}

//-----------------------------------------------------------------
//: Find the dominant peaks in the direction histogram.
//  The output vector contains the theta indices of dominant direction peaks.
//  \param thresh is the minimum number of lines in a valid peak.
//  \param angle tol is the width of the peak in degrees.
int bsol_hough_line_index::
dominant_directions(const int thresh, const float angle_tol,
                    std::vector<int>& dirs)
{
  int radius = int(0.5f+angle_tol/angle_increment_); // round to nearest int
  std::vector<int> angle_hist = this->angle_histogram();
  std::vector<int> suppressed_hist =
    this->non_maximum_suppress(radius, angle_hist);
  for (int i = 0; i<th_dim_; i++)
    if (suppressed_hist[i]>=thresh)
      dirs.push_back(i);
  return dirs.size();
}

//------------------------------------------------------------------
//: Get the dominant line groups in the hough index.
//  Sets of lines belonging to distinct orientations are returned.
//
int bsol_hough_line_index::
dominant_line_groups(const int thresh, const float angle_tol,
                     std::vector<std::vector<vsol_line_2d_sptr> >& groups)
{
  groups.clear();
  std::vector<int> dirs;
  int n_groups = this->dominant_directions(thresh, angle_tol, dirs);
  if (!n_groups)
    return 0;
  for (int gi = 0; gi<n_groups; gi++)
  {
    std::vector<vsol_line_2d_sptr> lines;
    float angle = dirs[gi]*angle_increment_;
    this->parallel_lines(angle, angle_tol, lines);
    groups.push_back(lines);
  }
  std::sort(groups.begin(), groups.end(), nlines());
  return n_groups;
}

//-----------------------------------------------------------------
//: An image of the hough index, useful for debugging applications that use this class
vbl_array_2d<unsigned char> bsol_hough_line_index::get_hough_image()
{
  vbl_array_2d<unsigned char> out(r_dim_, th_dim_);
  int nmax = 0;
  for (int r=0;r<r_dim_;r++)
    for (int th=0;th<th_dim_;th++)
    {
      int n_lines = index_[r][th]->size();
      if (n_lines>nmax)
        nmax = n_lines;
    }
  float scale = 1;
  if (nmax != 0)
    scale /= 1.0f/nmax;
  for (int r=0;r<r_dim_;r++)
    for (int th=0;th<th_dim_;th++)
    {
      unsigned char val;
      int n_lines = index_[r][th]->size();
      float v = 255*n_lines/scale;
      val = (unsigned char)v;
      out.put(r,th,val);
    }
  return out;
}
