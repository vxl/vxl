// This is brl/bseg/brip/brip_histogram.h
#ifndef brip_histogram_h_
#define brip_histogram_h_
//:
// \file
// \brief Calculate various special types of histograms
// \author Matt Leotta
//

#include <iostream>
#include <vector>
#include <vil/vil_image_view.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: Construct histogram from pixels in the given image.
//  same as vil_histogram except that it returns the sum
//  of all values in the histogram.
template<class T>
double brip_histogram(const vil_image_view<T>& image,
                      std::vector<double>& histo,
                      double min, double max, unsigned n_bins);


//: Construct weighted histogram from pixels in the given image using
//  values in an image of weights.  The sum of all weights used is returned.
template<class T>
double brip_weighted_histogram(const vil_image_view<T>& image,
                               const vil_image_view<double>& weights,
                               std::vector<double>& histo,
                               double min, double max, unsigned n_bins);


//: Construct the joint histogram between image1 and image2.
//  The the values of min, max, and n_bins are used on both images
//  the result is a 2D (n_bins x n_bins) std::vector.
//  The sum of all values in the histogram is returned.
template<class T>
double brip_joint_histogram(const vil_image_view<T>& image1,
                            const vil_image_view<T>& image2,
                            std::vector<std::vector<double> >& histo,
                            double min, double max, unsigned n_bins);

#endif // brip_histogram_h_
