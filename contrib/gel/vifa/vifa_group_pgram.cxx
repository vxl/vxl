// This is gel/vifa/vifa_group_pgram.cxx
#include <iostream>
#include <cmath>
#include "vifa_group_pgram.h"
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_clip.h>

#ifndef DEGTORAD
#define DEGTORAD  (vnl_math::pi_over_180)
#endif


vifa_group_pgram::
vifa_group_pgram(imp_line_list&                  lg,
                 const vifa_group_pgram_params&  old_params,
                 double                          angle_range)
  : vifa_group_pgram_params(old_params)
{
  angle_range_ = angle_range;
  th_dim_ = (int)std::ceil(angle_range_/angle_increment());
  th_dim_++; //Include both 0 and angle_range_
  bb_ = new vifa_bbox;

  for (int i = 0; i < th_dim_; i++)
  {
    auto*  illp = new imp_line_list;
    curves_.push_back(illp);
  }
  this->Index(lg);
}

//------------------------------------------------------------
//: Destructor
vifa_group_pgram::
~vifa_group_pgram()
{
  for (auto illp : curves_)
  {
    for (auto & ili : *illp)
    {
      ili = nullptr;
    }

    delete illp;
  }
}

//-----------------------------------------------------
//: Add an ImplicitLine to the index
void vifa_group_pgram::
Index(const imp_line_sptr& il)
{
  int  ang_bin = this->AngleLoc(il);
  curves_[ang_bin]->push_back(il);

  this->touch();
}

//------------------------------------------------------------
//: Add a set of ImplicitLines to the angular index
void vifa_group_pgram::
Index(imp_line_list& lg)
{
  for (auto & ili : lg)
    Index(ili);
}

//------------------------------------------------------------
//: Clear all lines from the index
void vifa_group_pgram::
Clear()
{
  for (auto & curve : curves_)
  {
    imp_line_list*  illp = curve;
    for (auto & ili : *illp)
      ili = nullptr;

    delete illp;
    curve = new imp_line_list;
  }

  this->touch();
}

//-----------------------------------------------------------------
//: Compute a histogram of parallel line coverage
//
vifa_histogram_sptr vifa_group_pgram::
GetCoverageHist(void)
{
  vifa_histogram_sptr  h = new vifa_histogram(th_dim_, 0.0f, float(angle_range_));

  float*  cnts = h->GetCounts();
  for (int i = 0; i < th_dim_; i++)
    cnts[i] = float(this->LineCoverage(i));
  return h;
}

//--------------------------------------------------------------
//: Get a populated line coverage corresponding to a given angle bin
//
vifa_line_cover_sptr vifa_group_pgram::
GetLineCover(int  angle_bin)
{
  imp_line_sptr  il = this->LineAtAngle(angle_bin);

  // Get all the lines which are within +- angular resolution of angle_bin.
  imp_line_list  lg;
  this->CollectAdjacentLines(angle_bin, lg);

  if (!lg.size())
  {
    return nullptr;
  }

  // Construct a bounding box from the ROI and clip
  // the line defined by angle_bin
  double  bx;
  double  by;
  double  ex;
  double  ey;
  this->CheckUpdateBoundingBox();
  if (!vgl_clip_line_to_box(il->a(), il->b(), il->c(),
                            bb_->min_x(), bb_->min_y(),
                            bb_->max_x(), bb_->max_y(),
                            bx, by, ex, ey))
  {
    std::cerr << "In vifa_group_pgram::GetLineCover(): No intersection found\n";
    return nullptr;
  }

  // Here we set the clipping bounds.
  vgl_point_2d<double>  b(bx, by);
  vgl_point_2d<double>  e(ex, ey);
  il->set_points(b, e);

  // The line is cut into 1 pixel bins
  int len = int(il->length());
  if (!len)
  {
    return nullptr;
  }

  vifa_line_cover_sptr  cov = new vifa_line_cover(il, len);
  for (auto & ili : lg)
  {
    cov->InsertLine(ili);
  }

  return cov;
}

//--------------------------------------------------------------
//: Compute parallel line overlap on a line at the same orientation with midpoint at the center of the region of the line group.
//
double vifa_group_pgram::
LineCoverage(int  angle_bin)
{
  vifa_line_cover_sptr  lc = this->GetLineCover(angle_bin);
  return lc ? lc->GetCoverage() : 0.0;
}

//--------------------------------------------------------------
//: Collect implicit line(s) from the angle array at orientations +- the given bin orientation.
//  Wrap around the end of the array so that 0 degrees and 180 degrees are considered parallel.
void vifa_group_pgram::
CollectAdjacentLines(int             angle_bin,
                     imp_line_list&  lg)
{
  if (angle_bin >= 0 && angle_bin < th_dim_)
  {
    if (angle_bin == 0)
    {
      // Case I - At the beginning of the array
      lg.insert(lg.end(), curves_[th_dim_ - 1]->begin(),
                curves_[th_dim_ - 1]->end());
      lg.insert(lg.end(), curves_[0]->begin(),
                curves_[0]->end());
      lg.insert(lg.end(), curves_[1]->begin(),
                curves_[1]->end());
    }
    else if (angle_bin == th_dim_ - 1)
    {
      // Case II - At the end of the array
      lg.insert(lg.end(), curves_[th_dim_ - 2]->begin(),
                curves_[th_dim_ - 2]->end());
      lg.insert(lg.end(), curves_[th_dim_ - 1]->begin(),
                curves_[th_dim_ - 1]->end());
      lg.insert(lg.end(), curves_[0]->begin(),
                curves_[0]->end());
    }
    else
    {
      // Case III - not near ends of the array
      lg.insert(lg.end(), curves_[angle_bin - 1]->begin(),
                curves_[angle_bin - 1]->end());
      lg.insert(lg.end(), curves_[angle_bin]->begin(),
                curves_[angle_bin]->end());
      lg.insert(lg.end(), curves_[angle_bin + 1]->begin(),
                curves_[angle_bin + 1]->end());
    }
  }
  else
    std::cerr << "vifa_group_pgram::CollectAdjacentLines(): bad angle_bin\n";
}

//------------------------------------------------------------------
//: Get Total length of parallel lines adjacent and including a bin
//
double vifa_group_pgram::
GetAdjacentPerimeter(int  bin)
{
  imp_line_list  lg;
  this->CollectAdjacentLines(bin, lg);

  double  sum = 0;
  for (auto & ili : lg)
  {
    vgl_vector_2d<double>  v = ili->point2() - ili->point1();
    sum += v.length();
  }

  return sum;
}

//------------------------------------------------------
//: Compute the total length of parallel lines normalized by the total edge perimeter
double vifa_group_pgram::
norm_parallel_line_length(void)
{
  this->ComputeDominantDirs();
  if (dominant_dirs_.size() < 1)
  {
    // No basis
    return 0.0;
  }

  double max_cover = 0.0;
  int    max_dir = 0;
  auto  iit = dominant_dirs_.begin();
  for (; iit != dominant_dirs_.end(); iit++)
  {
    int            dir = (*iit);
    vifa_line_cover_sptr  lc = this->GetLineCover(dir);
    if (!(lc.ptr()))
    {
//      std::cout << "vgg::norm_parallel_line_length(): "
//               << "No line cover found for dir " << dir << std::endl;

      // Is this right?  What about other dominant directions?
      // return 0.0;
      continue;
    }

    double  cov = lc->GetCoverage();

    if (cov > max_cover)
    {
      max_cover = cov;
      max_dir = dir;
    }
  }

  double  per = this->GetAdjacentPerimeter(max_dir);

//  std::cout << "vgg::norm_parallel_line_length(): per = " << per
//           << ", tmp1_ = " << tmp1_ << std::endl;

  return 1.5 * per / tmp1_;
}

//---------------------------------------------------------
//: Find the angle bin corresponding to an implicit_line
int vifa_group_pgram::
AngleLoc(const imp_line_sptr&  il)
{
  // Compute angle index
  double  angle = std::fmod(il->slope_degrees(), 180.0);
  if (angle < 0.0)
    angle += 180.0;

  if (angle > angle_range_)
  {
    std::cerr << "In vifa_group_pgram::AngleLoc(): angle " << angle
             << " was outside the angle range " << angle_range_ << std::endl;
    return 0;
  }

  return (int)(std::floor(angle / angle_increment()));
}

//--------------------------------------------------------------
//: Define a line passing through the center of the Hough ROI and at an angle corresponding to the angle bin
imp_line_sptr vifa_group_pgram::
LineAtAngle(int  angle_bin)
{
  // Get the new line's direction unit vector
  double          ang_rad = DEGTORAD * angle_bin * angle_increment();
  vgl_vector_2d<double>  d(std::cos(ang_rad), std::sin(ang_rad));

  // Get the new line's midpoint (bounding box centroid)
  this->CheckUpdateBoundingBox();
  vgl_point_2d<double>  m = bb_->centroid();

  // Return a new line
  imp_line_sptr      il = new imp_line(d, m);
  return il;
}

//------------------------------------------------------
//: Compute the bounding box of the current index
//
void vifa_group_pgram::
ComputeBoundingBox(void)
{
  // Reset the bounding box
  bb_->empty();

  for (auto illp : curves_)
  {
    for (auto & ili : *illp)
    {
      const imp_line_sptr&  il = ili;

      bb_->add(il->point1());
      bb_->add(il->point2());
    }
  }

  // Update the bounding box timestamp
  bb_->touch();
}

//---------------------------------------------------------
//: Compute the dominant directions using the coverage histogram.
//  A dominant direction is a local maximum in the coverage distribution.
//
void vifa_group_pgram::
ComputeDominantDirs(void)
{
  dominant_dirs_.clear();

  // Get the coverage histogram and do find local maxima.
  vifa_histogram_sptr  h = this->GetCoverageHist();
  vifa_histogram_sptr  h_sup = h->NonMaximumSupress(max_suppress_radius(), true);
  float*               cnts = h_sup->GetCounts();
  int                  max_idx = h_sup->GetRes();

  // Rebuild the dominant direction array
  for (int i = 0; i < max_idx; i++)
    if (cnts[i] > 0)
      dominant_dirs_.push_back(i);

//  std::cout << "vgg::ComputeDominantDirs(): max_idx = " << max_idx << ", "
//           << dominant_dirs_.size() << " dominant directions found\n";
}
