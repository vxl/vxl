// This is brl/bbas/bsta/bsta_detector_gaussian.h
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
//   (none yet)
// \endverbatim

//: A simple Mahalanobis distance detector for a Gaussian
//  Detects samples that lie within some Mahalanobis distance
template <class gaussian_>
class bsta_g_mdist_detector
{
  public:
    typedef bool return_T;
    enum { return_dim = 1 };
    typedef typename gaussian_::math_type T;
    typedef typename gaussian_::vector_type vector_;

    //: Constructor
    bsta_g_mdist_detector(const T& thresh=T(2.5)) : sqr_threshold(thresh*thresh) {}

    //: The main function
    // \retval true if the Mahalanobis distance is less than the threshold
    bool operator() (const gaussian_& g, const vector_& sample, bool& result) const
    {
      result = g.sqr_mahalanobis_dist(sample) < sqr_threshold;
      return true;
    }

    //: the threshold on Mahalanobis distance
    T sqr_threshold;
};


#endif // bsta_detector_gaussian_h_
