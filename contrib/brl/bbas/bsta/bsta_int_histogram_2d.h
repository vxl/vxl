// This is brl/bbas/bsta/bsta_int_histogram_2d.h
#ifndef BSTA_INT_HISTOGRAM_2D_H_
#define BSTA_INT_HISTOGRAM_2D_H_

//-----------------------------------------------------------------------------
//:
// \file
// \brief 1D and 2D integer Histograms with bucket width = 1
//
//  This simple histogram class is an integer version of gel/vifa/vifa_histogram.h
//    which was a port from Targetjr.  Also borrowed from bsta_histogram.  Only the
//    features needed for integer histograms of images with bucket_width = 1 are included.
//    The class is defined to create a histogram with long int buckets (4 bytes).
//  Note that the 2D version is lacking some of the 1D methods because it isn't obvious
//    that they would be of any use for a 2D histogram
//  The 2D histogram is a special case useful for isolating simularly colored regions 
//    of an image.  Probably not very generally useful to the vxl community.  But here 
//    it is anyway
//  This is initally placed in contrib/jgl/bsta for now.  If others think it useful, it can be
//    moved to a permanent home later.
//
// \author James E. Green
// \date   23 April, 2007
//
// \verbatim
//  Modifications
//   2007/04/23 Initial Version
//
// \endverbatim
//-----------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <bsta/bsta_int_histogram_1d.h>

class bsta_int_histogram_2d
{

 private:
  unsigned int nbins_x_;
  unsigned int nbins_y_;
  unsigned int diag_;								// # buckets in diagonal 1D hist
  vcl_vector<vcl_vector<long int> > counts_;		// the histogram buckets
  
 public:

 //:default constructor that assumes all data values are positive
  // constructor parameters:
  //   nbins_x, nbins_y      # bins to create in this 2D histogram.
  bsta_int_histogram_2d(const unsigned int nbins_x, const unsigned int nbins_y);

  // destructor
  ~bsta_int_histogram_2d() ;

  // The number of bins in the histogram
  unsigned int nbins_x() const { return nbins_x_;}
  unsigned int nbins_y() const { return nbins_y_;}

  //: get the count in a given bin
  long int get_count(const unsigned int binx, const unsigned int biny)
    {assert(binx < nbins_x_ && biny < nbins_y_); return counts_[biny][binx];}

  //: set the count in a given bin
  void set_count(const unsigned int binx, const unsigned int biny, const long int count)
    {if(binx>=0 && binx<nbins_x_ && biny>=0 && biny<nbins_y_) counts_[biny][binx] = count;}
    
  //: Total area under the histogram = total counts in histogram
  unsigned long int get_area();

  // get highest value in histogram; returns max value; index of max is available in imax
  unsigned long int get_max_val(unsigned int &imax_x, unsigned int &imax_y);

  //: Smooth the 2D histogram with a Parzen window of sigma
  void parzen(const float sigma);

  //: Form a "profile" histogram along the diagonal with max value normal to 
  //   diagonal in buckets
  void profile_histogram( bsta_int_histogram_1d &phist, 
						  bsta_int_histogram_1d &phist_x,
						  bsta_int_histogram_1d &phist_y );

  // Find where peak drops to X% along normal on either front or rear edge of 
  //   diagonal slope.  Here the "front is the top edge, "rear" is the bottom edge 
  bool find_edge( unsigned int peak_y, unsigned int peak_x, 
		      float newslope, float edgepct, 
		      unsigned int &edge_x, unsigned int &edge_y,
			  bool front);
};

#endif // BSTA_INT_HISTOGRAM_2D_H_





