// This is brcv/seg/bsta/algo/bsta_detector_gaussian.h
#ifndef bsta_detector_gaussian_h_
#define bsta_detector_gaussian_h_

//:
// \file
// \brief Detectors applying to Gaussians  
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 02/09/06
//
// \verbatim
//  Modifications
// \endverbatim

#include <vnl/vnl_vector_fixed.h>

//: A simple Mahalanobis distance detector for a Gaussian
// detects samples that lie within some Mahalanobis distance
template <class _gaussian>
class bsta_g_mdist_detector
{
  public:
    typedef bool return_T;
    enum { return_dim = 1 };
    typedef typename _gaussian::math_type T;
    typedef typename _gaussian::vector_type _vector;

    //: Constructor
    bsta_g_mdist_detector(const T& thresh=T(2.5)) : sqr_threshold(thresh*thresh) {}

    //: The main function
    // \retval true if the Mahalanobis distance is less than the threshold
    bool operator() (const _gaussian& g, const _vector& sample, bool& result) const
    {
      result = g.sqr_mahalanobis_dist(sample) < sqr_threshold;
      return true;
    }

    //: the threshold on Mahalanobis distance
    T sqr_threshold;
};




#endif // bsta_detector_gaussian_h_
