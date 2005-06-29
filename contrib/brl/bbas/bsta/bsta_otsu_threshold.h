#ifndef _bsta_otsu_threshold_h_
#define _bsta_otsu_threshold_h_
//:
// \file
// \brief Implements Otsu's threshold method for 1D distribution and 2 classes
// \author Vishal Jain, (vj@lems.brown.edu)
// \date 04/09/05
//
//  The Otsu threholding method maximizes the between class variance by
//  an exhaustive linear scan over the histogram bin indices
//
// \verbatim
//  Modifications  Templated and moved up to bsta - J.L. Mundy June 29, 2005
// \endverbatim
#include<vcl_vector.h>
#include<bsta/bsta_histogram.h>
template <class T> class bsta_otsu_threshold{

public:
    //: constructor
    bsta_otsu_threshold(bsta_histogram<T> const& hist);

    //: destructor
    ~bsta_otsu_threshold();

    unsigned bin_threshold();
    
    T threshold()
      {return distribution_1d_.avg_bin_value(this->bin_threshold());}

protected:
    //: default constructor doesn't make sense
    bsta_otsu_threshold() : distribution_1d_(bsta_histogram<T>(0,0)){}

    bsta_histogram<T> distribution_1d_;
};

#define BSTA_OTSU_THRESHOLD_INSTANTIATE(T) extern "Please #include <bsta/bsta_otsu_threshold.txx>"


#endif // bsta_otsu_threshold_h_
