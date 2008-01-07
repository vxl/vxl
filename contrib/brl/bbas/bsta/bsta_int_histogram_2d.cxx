// This is brl/bbas/bsta/bsta_int_histogram_2d.cxx
// see bsta/bsta_int_histogram_2d.h for description of class
#include "bsta_int_histogram_2d.h"
//:
// \file
#include <vcl_cmath.h>
// for gausian parzan window filter
#include "bsta_gauss.h"
#include "bsta_int_histogram_1d.h"

// constructor
bsta_int_histogram_2d::bsta_int_histogram_2d(const unsigned int nbins_x, const unsigned int nbins_y)
{
  nbins_x_ = nbins_x;
  nbins_y_ = nbins_y;
  float diag_lgth = vcl_sqrt( static_cast<float>(nbins_x_*nbins_x_) + static_cast<float>(nbins_y_*nbins_y_));
  diag_ = static_cast<long int>(diag_lgth) + 2;

// create the array for histogram nbins_x_ by nbins_y_ wide
  counts_.resize(nbins_y_);
  for (unsigned row =0; row<nbins_y_; ++row)
  {
    vcl_vector<long int> temp(nbins_x_, 0); // zero out all bins (always a good idea)
    counts_[row]=temp;
  }
}

// destructor
bsta_int_histogram_2d::~bsta_int_histogram_2d() {}

// ------------------------------------------------
// get total counts in entire histogram
unsigned long int bsta_int_histogram_2d::get_area()
{
  register unsigned long int area = 0;
  for (unsigned int j=0; j<nbins_y_; j++) {
    for (unsigned int i=0; i<nbins_x_; i++) {
      area = area + counts_[j][i];
    }
  }
    return area;
}

// (get_counts() and set_counts() defined as inline in .h file

// get highest value in histogram; returns max value; index of max is available in imax
unsigned long int bsta_int_histogram_2d::get_max_val(unsigned int &imax, unsigned int &jmax)
{
  register long int max = 0;
  for (unsigned int j=0; j<nbins_y_; j++)
  {
    for (unsigned int i=0; i<nbins_x_; i++)
    {
      if (counts_[j][i] > max)
      {
        max = counts_[j][i];
        imax = i;
        jmax = j;
      }
    }
  }
return max;
}


// smooth the histogram with a 2D parzan window (which is a gaussian filter)
void bsta_int_histogram_2d::parzen(const float sigma)
{
  if (sigma<=0)
    return;
  double sd = (double)sigma;
  double val = 0.0;
  int nx = nbins_x_;  // arguments to create vbl_array_2d are ints
  int ny = nbins_y_;
  vbl_array_2d<double> in(nx, ny), out(nx, ny);
  for (unsigned int j=0; j<nbins_y_; j++)
  {
    for (unsigned int i=0; i<nbins_x_; i++)
    {
      val = (double)(counts_[j][i]);
      in.put((int)i, (int)j, val);
    }
  }
  bsta_gauss::bsta_2d_gaussian(sd, in, out);
  for (unsigned int j=0; j<nbins_y_; j++)
  {
    for (unsigned int i=0; i<nbins_x_; i++)
    {
      // as we are going back to a long int, round off here
      val = out.get((int)i, (int)j);
      counts_[j][i] = (unsigned int)(val +0.5);
    }
  }
  return;
}


//: Form a profile histogram with max value normal to diagonal buckets
void bsta_int_histogram_2d::profile_histogram( bsta_int_histogram_1d &phist,
                                               bsta_int_histogram_1d &phist_x,
                                               bsta_int_histogram_1d &phist_y )
{
  // Calculate slope and increments along diagonal
  float slope = static_cast<float>(nbins_y_)/static_cast<float>(nbins_x_);
  float inverse_slope = 1.0f/slope;
#if 0 // unused variables ?!
  float diag_lgth = vcl_sqrt(1.0f + (inverse_slope*inverse_slope));
  float deltay = inverse_slope/diag_lgth;
  float deltax = 1.0f/diag_lgth;
#endif
  // find intercepts of slopes and calculate box that must be examined
  float dxintcpt = vcl_sqrt(1 + (slope*slope));
  float dyintcpt = vcl_sqrt(1 + (inverse_slope*inverse_slope));
  unsigned int xbox = static_cast<unsigned int>(vcl_ceil(dxintcpt));
  unsigned int ybox = static_cast<unsigned int>(vcl_ceil(dyintcpt));

  // For each bucket in the diagonal histogram, search normal to the diagonal
  //   in the 2D histogram for a "max" value.
  // Step along the diagonal i.  Project the normal to the diagonal onto the X
  //   axis and step up the normal line to the Y axis.  Start filling phist at
  //   the phist[0] bin and go up to end of phist.  But don't search outside of
  //   the 2D histogram.

  for (unsigned int i=0; i<diag_; i++)
  {
    phist.set_count(i, 0);                // starting value for normal line
    float xaxis_proj = vcl_sqrt((i*i) + ((i*slope)+(i*slope)));
    float yaxis_proj = vcl_sqrt((i*i) + ((i/slope)+(i/slope)));

    // starting position on X axis
    float xpos = xaxis_proj;            // start on X axis and step up to Y axis
    float ypos = 0.0;                    // start at Y = 0.0

    // Only try to test 2D hist bucket if projection falls in its range.
    //   At the high end of phist[] most projections will fall out of range.
    while (xpos >= 0.0 && ypos < yaxis_proj)
    {
      unsigned int xindex = static_cast<int>(vcl_floor(xpos));        // integer projection onto 2D axis
      unsigned int yindex = static_cast<int>(vcl_floor(ypos));

      // The integer diagonal normal line will miss some buckets in the 2D hist
      //   unless a "box" of buckets is tested for each bucket on the normal of
      //   the diagonal hist, so ...
      for (unsigned int j=0; j<ybox-1; j++)
      {
        unsigned int yi = yindex + j;
        for (unsigned int k=0; k<xbox-1; k++)
        {
          unsigned int xi = xindex + k;

          // Smoothed 2D hist range is [0:nbins_x_-1, 0:nbins_y_-1].  Loop starts
          //   at X = xaxis_proj and decreases down to 0 while Y starts at 0
          //   and increases up to yaxis_proj.  However, we only test smoothed
          //   2D buckets within the range of [0:nbins_x_-1, 0:nbins_y_-1].
          if (xi < nbins_x_ && yi < nbins_y_)
          {
            if (phist.get_count(i) < counts_[yi][xi])
            {
              phist.set_count(i, counts_[yi][xi]);
              phist_x.set_count(i, xi);
              phist_y.set_count(i, yi);
            }
          }
        }
      }
    }
  }

  return;
}


 // Find where peak drops to X% along normal on either front or rear edge of
 //   diagonal slope.  Here the "front is the top edge, "rear" is the bottom edge
 bool bsta_int_histogram_2d::find_edge( unsigned int peak_y, unsigned int peak_x,
                                      float newslope, float edge_pct,
                                      unsigned int &edge_x, unsigned int &edge_y,
                                      bool front)
{
  bool success = false;

  long int peak_height = counts_[peak_y][peak_x];        // the highest peak of the 2D histogram
  if (peak_height == 0) return success;                // exit if peak is zero
  long int limit = static_cast<long int>(peak_height * edge_pct);    // Value to reach to determine edge

  float diag_lgth = vcl_sqrt(1.0f + (newslope*newslope));
  float delta_x = 1.0f / diag_lgth;
  float delta_y = newslope / diag_lgth;

  float x_point = static_cast<float>(peak_x);
  float y_point = static_cast<float>(peak_y);

  // Test to make sure we are checking within range of 2D histogram
  while (x_point >= 0 && x_point < nbins_x_ && y_point >=0 && y_point < nbins_y_ )
  {
    if (front)        // look at top side of peak, Y increases, X decreases
    {
      x_point -= delta_x;
      y_point += delta_y;
    }
    else            // look at bottom side of peak, X increases, Y decreases
    {
      x_point += delta_x;
      y_point -= delta_y;
    }
    edge_x = static_cast<unsigned int>(x_point);    // Convert to unsigned int
    edge_y = static_cast<unsigned int>(y_point);

    if ( counts_[edge_y][edge_x] <= limit)            // Have we have reached edge?
    {
      success = true;
      return success;
    }
  }

  // failed to find edge, some perverse situation, return success = false
  return success;
}
