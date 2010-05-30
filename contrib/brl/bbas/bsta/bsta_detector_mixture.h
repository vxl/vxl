// This is brl/bbas/bsta/bsta_detector_mixture.h
#ifndef bsta_detector_mixture_h_
#define bsta_detector_mixture_h_
//:
// \file
// \brief Detectors applying to mixtures
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date February 09, 2006
//
// \verbatim
//  Modifications
//   (none yet)
// \endverbatim


//: Apply a detector to each component in order while the total weight is below a threshold.
//  Return true if any tested component matches
template <class mixture_, class detector_>
class bsta_top_weight_detector
{
  public:
    typedef bool return_T;
    enum { return_dim = 1 };
    typedef typename mixture_::math_type T;
    typedef typename mixture_::vector_type vector_;

    // for compatibility with vpdl/vdpt
    typedef return_T return_type;
    typedef mixture_ distribution_type;

    //: Constructor
    bsta_top_weight_detector(const detector_& d, const T& w=T(0.5))
      : detect(d), weight_thresh(w) {}

    //: The main function
    bool operator() (const mixture_& mix, const vector_& sample, bool& result) const
    {
      T total_weight = T(0);
      result = false;
      for (unsigned int i=0; i<mix.num_components(); ++i){
        if (total_weight > weight_thresh)
          return true;
        if ( !detect(mix.distribution(i),sample,result) )
          return false;
        if (result)
          return true;
        total_weight += mix.weight(i);
      }
      return true;
    }

    //: The detector to apply to components
    detector_ detect;
    //: The index to detect
    T weight_thresh;
};


//: Apply a detector to each component in order while the total weight is below a threshold.
// Return true if any tested component matches
template <class mixture_, class detector_>
class bsta_mix_any_less_index_detector
{
  public:
    typedef bool return_T;
    enum { return_dim = 1 };
    typedef typename mixture_::math_type T;
    typedef typename mixture_::vector_type vector_;

    // for compatibility with vpdl/vdpt
    typedef return_T return_type;
    typedef mixture_ distribution_type;

    //: Constructor
    bsta_mix_any_less_index_detector(const detector_& d, const T& w=T(0.5))
      : detect(d), weight_thresh(w) {}

    //: The main function
    bool operator() (const mixture_& mix, const vector_& sample, bool& result) const
    {
      T total_weight = T(0);
      result = false;

      bool flag=false;
      for (unsigned int i=0; i<mix.num_components(); ++i){
        if (mix.weight(i) > weight_thresh)
        {
          flag=true;
          if ( !detect(mix.distribution(i),sample,result) )
            return false;
          if (result)
            return true;
        }
        //total_weight += mix.weight(i);
      }

      return !flag;
    }

    //: The detector to apply to components
    detector_ detect;
    //: The index to detect
    T weight_thresh;
};

#endif
