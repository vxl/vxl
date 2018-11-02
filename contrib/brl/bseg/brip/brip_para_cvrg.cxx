#include <cstdlib>
#include <iostream>
#include <cmath>
#include "brip_para_cvrg.h"
//:
// \file

#include <vul/vul_timer.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include "brip_vil_float_ops.h"

//-----------------------------------------------------------------------
//: Variable Initialization
void brip_para_cvrg::init_variables()
{
  width_ = int(3*sigma_);
  proj_n_ = 2*proj_width_ + 1;
  sup_proj_ = std::vector<float>(proj_n_, 0.0f);
  proj_0_ = std::vector<float>(proj_n_, 0.0f);
  proj_45_ = std::vector<float>(proj_n_, 0.0f);
  proj_90_ = std::vector<float>(proj_n_, 0.0f);
  proj_135_ = std::vector<float>(proj_n_, 0.0f);
}


//-----------------------------------------------------------------------
//:  Image Initialization
void brip_para_cvrg::init(vil_image_resource_sptr const & image)
{
  vul_timer t;
  // we don't have roi capability so just use the whole image for now
  int w = image->ni(), h = image->nj();
  xstart_ = 0;
  ystart_ = 0;
  xsize_ = w;
  ysize_ = h;

  std::cout << "xstart = " << xstart_ << " ystart_ = " << ystart_ << '\n'
           << "xsize = " << xsize_ << " ysize_ = " << ysize_ << '\n'
           << std::flush;

  image_ = brip_vil_float_ops::convert_to_float(image);
  avg_.set_size(w,h);
  grad0_.set_size(w,h);
  grad45_.set_size(w,h);
  grad90_.set_size(w,h);
  grad135_.set_size(w,h);
  det_.set_size(w,h);
  dir_.set_size(w,h);
  this->init_variables();
  std::cout << "Do Initialization in " << t.real() << " msecs\n"
           << std::flush;
}

//-----------------------------------------------------------------------------
//: Constructor s
brip_para_cvrg::brip_para_cvrg(float sigma, float thresh,
                               float gauss_tail, int proj_width,
                               int proj_height, int sup_radius,
                               bool v) :
  brip_para_cvrg_params(sigma, thresh, gauss_tail, proj_width, proj_height,
                        sup_radius, v)
{
  this->init_variables();
}

brip_para_cvrg::brip_para_cvrg(brip_para_cvrg_params& pdp) :
  brip_para_cvrg_params(pdp)
{
  this->init_variables();
}

//-----------------------------------------------------------------------------
//: Destructor.
brip_para_cvrg::~brip_para_cvrg()
= default;

//-----------------------------------------------------------------------------
//: Convolves the image with the smoothing kernel.  Private.
void brip_para_cvrg::smooth_image()
{
  vul_timer t;
  smooth_ = brip_vil_float_ops::gaussian(image_, sigma_);
  std::cout << "Smooth image in " << t.real() << " msecs\n"
           << std::flush;
}


//----------------------------------------------------------
//: Compute the average value in the 7x7 window
void brip_para_cvrg::avg(int x, int y, vil_image_view<float> const& smooth, vil_image_view<float>& avg)
{
  float sum =0;
  for (int i = -3; i<=3; i++)
    for (int j = -3; j<=3; j++)
    {
      sum += smooth(x+i, y+j);
    }

  avg(x,y) = sum/49.0f;
}


//----------------------------------------------------------
//: Compute a gradient operator at x,y along the x axis
void brip_para_cvrg::grad0(int x, int y, vil_image_view<float> const& smooth,
                           vil_image_view<float>& grad0)
{
  float plus = 0.5f*smooth(x+1,y+3) + smooth(x+1,y+2) + smooth(x+1,y+1) +
    smooth(x+1,y) + smooth(x+1,y-1) + smooth(x+1,y-2)
    + 0.5f*smooth(x+1,y-3);
  float minus = 0.5f*smooth(x-1,y+3) + smooth(x-1,y+2) + smooth(x-1,y+1) +
    smooth(x-1,y) + smooth(x-1,y-1) + smooth(x-1,y-2) + 0.5f*smooth(x-1,y-3);
  grad0(x,y) = (plus-minus);
}


//----------------------------------------------------------
//: Compute a gradient operator at x,y at 45 degrees
void brip_para_cvrg::grad45(int x, int y, vil_image_view<float> const& smooth,
                            vil_image_view<float>& grad45)
{
  float plus = smooth(x-2,y+3) + smooth(x-1,y+2) + smooth(x,y+1)
    + smooth(x+1,y) + smooth(x+2,y-1) + smooth(x+3,y-2);
  float minus = smooth(x-3,y+2) + smooth(x-2,y+1) + smooth(x-1,y)
    + smooth(x,y-1) + smooth(x+1,y-2) + smooth(x+2,y-3);

  grad45(x,y) = 1.30f*(plus-minus);
}


//----------------------------------------------------------
//: Compute a gradient operator at x,y at 90 degrees
void brip_para_cvrg::grad90(int x, int y, vil_image_view<float> const& smooth,
                            vil_image_view<float>& grad90)
{
  float plus = 0.5f*smooth(x+3,y+1) + smooth(x+2,y+1) + + smooth(x+1,y+1) +
    smooth(x,y+1) + smooth(x-1,y+1) + smooth(x-2,y+1) + 0.5f*smooth(x-3,y+1);
  float minus =0.5f*smooth(x+3,y-1) + smooth(x+2,y-1)+ smooth(x+1,y-1) +
    smooth(x,y-1) + smooth(x-1,y-1) + smooth(x-2,y-1) + 0.5f*smooth(x-3,y-1);
  grad90(x, y) = (plus-minus);
}


//----------------------------------------------------------
//: Compute a gradient operator at x,y at 135 degrees
void brip_para_cvrg::grad135(int x, int y, vil_image_view<float> const& smooth,
                             vil_image_view<float>& grad135)
{
  float plus = smooth(x+3,y+2) + smooth(x+2,y+1) + smooth(x+1,y)
    + smooth(x,y-1) + smooth(x-1,y-2) + smooth(x-2,y-3);
  float minus = smooth(x+2,y+3) + smooth(x+1,y+2) + smooth(x,y+1)
    + smooth(x-1,y) + smooth(x-2,y-1) + smooth(x-3,y-2);

  grad135(x, y) = 1.3f*(plus-minus);
}


//-----------------------------------------------------------------------------
//: Convolves with the kernel in the x direction, to compute the local derivative in that direction.
//  Private.
void brip_para_cvrg::compute_gradients()
{
  vul_timer t;
  int x,y;
  int radius = width_+3;
  grad0_.fill(0.0f);
  grad45_.fill(0.0f);
  grad90_.fill(0.0f);
  grad135_.fill(0.0f);
  for (y=radius;y<ysize_ - radius;y++)
    for (x=radius;x<xsize_ - radius;x++)
    {
      this->avg(x, y, smooth_, avg_);
      this->grad0(x, y, smooth_, grad0_);
      this->grad45(x, y, smooth_, grad45_);
      this->grad90(x, y, smooth_, grad90_);
      this->grad135(x, y, smooth_, grad135_);
    }
  std::cout << "Compute gradients in " << t.real() << " msecs\n"
           << std::flush;
}


//------------------------------------------------------------
//: Project the gradient magnitude along a given direction.
//  The result is a 1-d projection plot.
//  \verbatim
//                     .
//                    *  .
//                   ^  *  .
//                 /   ^  *  .
//                  \    ^  *  .
//                         ^  *  \ .
//   2*proj_width_+1   x     ^  x-----2*proj_height_+1
//                        \ / \ .
// \endverbatim
float brip_para_cvrg::project(int x, int y, int dir,
                              std::vector<float>& projection)
{
  int w,h;
  int w0 = proj_width_;
  // float energy = 0.0f;
  for (h=-proj_height_; h<=proj_height_; h++)
    for (w=-w0; w<=w0; w++)
    {
      switch (dir)
      {
      case 0:
        projection[w+w0] += grad0_(x+w,y+h);
        break;
      case 45:
        projection[w+w0] += grad45_(x+h+w,y+w-h);
        break;
      case 90:
        projection[w+w0] += grad90_(x+h,y+w);
        break;
      case 135:
        projection[w+w0] += grad135_(x+h-w,y+w+h);
        break;
      default:
        projection[w+w0] += 0; // no-op
        break;
      }
    }
  float max_energy = 0;
  for (int i =0; i<proj_n_; i++)
  {
    float val = std::fabs(projection[i]);
    if (val>max_energy)
      max_energy = val;
  }
  return max_energy;
}


//: Prune any sequences of more than one maximum value.
// That is, it is possible to have a "flat" top peak with an arbitrarily
// long sequence of equal, but maximum values.
//
void brip_para_cvrg::remove_flat_peaks(int n, std::vector<float>& array)
{
  int nbm = n-1;

  // Here we define a small state machine - parsing for runs of peaks
  // init is the state corresponding to an initial run (starting at i ==0)
  bool init= array[0]==0;
  int init_end =0;

  // start is the state corresponding to any other run of peaks
  bool start=false;
  int start_index=0;

  // The scan of the state machine
  for (int i = 0; i < n; i++)
  {
    float v = array[i];

    // State init: a string of non-zeroes at the beginning.
    if (init&&v!=0)
      continue;

    if (init&&v==0)
    {
      init_end = i;
      init = false;
      continue;
    }

    // State !init&&!start: a string of "0s"
    if (!start&&v==0)
      continue;

    // State !init&&start: the first non-zero value
    if (!start&&v!=0)
    {
      start_index = i;
      start = true;
      continue;
    }
    // State ending flat peak: encountered a subsequent zero after starting
    if (start&&v==0)
    {
      int peak_location = (start_index+i-1)/2; // The middle of the run
      for (int k = start_index; k<=(i-1); k++)
        if (k!=peak_location)
          array[k] = 0;
      start = false;
    }
  }
  // Now handle the boundary conditions
  if (init_end!=0)  // Was there an initial run of peaks?
  {
    int init_location = (init_end-1)/2;
    for (int k = 0; k<init_end; k++)
      if (k!=init_location)
        array[k] = 0;
  }
  if (start)       // Did we reach the end of the array in a run of pks?
  {
    int end_location = (start_index + nbm)/2;
    for (int k = start_index; k<n; k++)
      if (k!=end_location)
        array[k] = 0;
  }
}


//------------------------------------------------------------------
//: Find locally maximum peaks in the input array
void brip_para_cvrg::non_maximum_supress(std::vector<float> const& input_array,
                                         std::vector<float>& sup_array)
{
  if ((2*sup_radius_ +1)> proj_width_)
  {
    std::cout << "In brip_para_cvrg::NonMaximumSupress(..) the kernel is too large\n";
  }
  std::vector<float> tmp(proj_n_);
  for (int i=0; i<proj_n_; i++)
    tmp[i]=std::fabs(input_array[i]);
  // Get the counts array of "this"
  // Make a new Histogram for the suppressed

  for (int i = sup_radius_; i < (proj_n_-sup_radius_); i++)
  {
    // find the maximum value in the current kernel
    float max_val = 0;
    for (int k = -sup_radius_; k <= sup_radius_ ;k++)
    {
      int index = i+k;
      if (tmp[index] > max_val)
        max_val = tmp[index];
    }
    // Is position i a local maximum?
    if (std::fabs(max_val-tmp[i])<1e-03)
      sup_array[i] = max_val; // Yes. So set the counts to the max value
  }
  this->remove_flat_peaks(proj_n_, sup_array);
}


//---------------------------------------------------------------
//: Find the amount of overlapping parallel coverage
float brip_para_cvrg::parallel_coverage(std::vector<float> const& input_array)
{
  sup_proj_.resize(proj_n_, 0.0f);
  this->non_maximum_supress(input_array, sup_proj_);
  int n_peaks = 0;
  float proj_sum = 0;
  for (int i = 0; i<proj_n_; i++)
    if (sup_proj_[i]>0)
    {
      n_peaks++;
      proj_sum += sup_proj_[i];
    }
  if (n_peaks<2)
    return 0;
  return proj_sum/float(n_peaks);
}


//----------------------------------------------------------
//: Find the direction with maximum parallel coverage.  Return the normalized coverage value.
void brip_para_cvrg::compute_parallel_coverage()
{
  vul_timer t;
  // float min_sum = .01f;
  det_.fill(0.0f);
  dir_.fill(0.0f);
  float direct;
  int radius = proj_width_+proj_height_ + 3;

  for (int y=radius; y<(ysize_-radius);y++)
    for (int x=radius ;x<(xsize_-radius);x++)
    {
      // zero arrays
      proj_0_ = std::vector<float>(proj_n_, 0.0f);
      proj_45_ = std::vector<float>(proj_n_, 0.0f);
      proj_90_ = std::vector<float>(proj_n_, 0.0f);
      proj_135_ = std::vector<float>(proj_n_, 0.0f);
      float coverage[4];
      this->project(x, y, 0, proj_0_);
      coverage[0] = this->parallel_coverage(proj_0_);
      float max_coverage = coverage[0];
      direct = 0.f;
      this->project(x, y, 45, proj_45_);
      coverage[1] = this->parallel_coverage(proj_45_);
      if (coverage[1]>max_coverage)
      {
        max_coverage = coverage[1];
        direct = 45.f;
      }
      this->project(x, y, 90, proj_90_);
      coverage[2] = this->parallel_coverage(proj_90_);
      if (coverage[2]>max_coverage)
      {
        max_coverage = coverage[2];
        direct = 90.f;
      }
      this->project(x, y, 135, proj_135_);
      coverage[3] = this->parallel_coverage(proj_135_);
      if (coverage[3]>max_coverage)
      {
        max_coverage = coverage[3];
        direct = 135.f;
      }
#ifdef DEBUG
      std::cout << '(' << x << ',' << y << ") coverage:\n"
               << "   O degrees = " << coverage[0] << '\n'
               << "  45 degrees = " << coverage[1] << '\n'
               << "  90 degrees = " << coverage[2] << '\n'
               << " 135 degrees = " << coverage[3] << '\n'
               << "max_coverage = " << max_coverage << '\n';
#endif

      det_(x,y) = max_coverage;
      dir_(x,y) = direct;
    }
  std::cout << "Do parallel coverage in " << t.real() << " msecs\n"
           << std::flush;
}


//------------------------------------------------------------------
//: Compute a 8-bit image from the projected gradients
void brip_para_cvrg::compute_image(vil_image_view<float> const& data,
                                   vil_image_view<unsigned char>& image)
{
  image = vil_image_view<unsigned char>(xsize_, ysize_);
  // find the maximum value
  float max_val = 0;
  for (int y = 0; y<ysize_; y++)
    for (int x = 0; x<xsize_; x++)
      if (data(x,y)>max_val)
        max_val = data(x,y);
  if (max_val<1e-06)
    max_val = 1e-06f;
  // Normalize the data and load the image
  for (int y = 0; y<ysize_; y++)
    for (int x = 0; x<xsize_; x++)
    {
      float temp = 255*data(x,y)/max_val;
      unsigned char val;
      val = (unsigned char)temp;
      image(x, y)=val;
    }
}


void brip_para_cvrg::do_coverage(vil_image_resource_sptr const& image)
{
  this->init(image);
  this->smooth_image();
  this->compute_gradients();
  this->compute_parallel_coverage();
}


//------------------------------------------------------------
//: Get the float image of detections. Scale onto [0, max]
vil_image_view<float>
brip_para_cvrg::get_float_detection_image(const float max)
{
  vil_image_view<float> out(xsize_, ysize_);
  out.fill(0);
  float max_val = 0;
  for (int y = 0; y<ysize_; y++)
    for (int x = 0; x<xsize_; x++)
      if (det_(x,y)>max_val)
        max_val = det_(x,y);

  if (max_val==0)
    return out;
  float s = max/max_val;
  for (int y = 0; y<ysize_; y++)
    for (int x = 0; x<xsize_; x++)
      out(x,y) = s*det_(x,y);
  return out;
}


//------------------------------------------------------------
//: Get the unsigned char image of detections
vil_image_view<unsigned char> brip_para_cvrg::get_detection_image()
{
  if (!det_image_)
    this->compute_image(det_, det_image_);
  return det_image_;
}


//------------------------------------------------------------
//: Get the direction image (unsigned char)
vil_image_view<unsigned char>  brip_para_cvrg::get_dir_image()
{
  if (!dir_image_) {
    dir_image_ = vil_image_view<unsigned char>(xsize_, ysize_);
  }
  for (int y = 0; y<ysize_; y++)
    for (int x = 0; x<xsize_; x++)
      dir_image_(x,y) = static_cast<unsigned char>(dir_(x,y));
  return dir_image_;
}

//------------------------------------------------------------
//: Get the combination of coverage and direction as a color image
vil_image_view<vil_rgb<unsigned char> >
brip_para_cvrg::get_combined_image()
{
  // "arbitrary" color assignments to the 4 directions: cyan,yellow,green,red:
  unsigned char r[4] ={0, 255, 0, 255};
  unsigned char g[4] ={255, 0, 255, 0};
  unsigned char b[4] ={255, 255, 0, 0};
  vil_image_view<unsigned char> cvrg_image = this->get_detection_image();
  vil_image_view<unsigned char> dir_image = this->get_dir_image();
  vil_image_view<vil_rgb<unsigned char> > out(xsize_, ysize_);
  for (int y = 0; y<ysize_; y++)
    for (int x = 0; x<xsize_; x++)
    {
      unsigned int direct = ((unsigned int)dir_image(x,y))/45;
      //      assert (direct<=3);
      if (direct>3) continue;
      unsigned char c = cvrg_image(x,y),
                    red  = r[direct]&c,
                    green= g[direct]&c,
                    blue = b[direct]&c;
      out(x, y) = vil_rgb<unsigned char>(red, green, blue);
    }
  return out;
}
