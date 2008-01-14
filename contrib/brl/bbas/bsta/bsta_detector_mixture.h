// This is brl/bbas/bsta/algo/bsta_detector_mixture.h
#ifndef bsta_detector_mixture_h_
#define bsta_detector_mixture_h_

//:
// \file
// \brief Detectors applying to mixtures 
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 02/09/06
//
// \verbatim
//  Modifications
// \endverbatim



//: Apply a detector to each component in order 
// while the total weight is below a threshold.
// Return true if any tested component matches
template <class _mixture, class _detector>
class bsta_top_weight_detector
{
  public:
    typedef bool return_T;
    enum { return_dim = 1 };
    typedef typename _mixture::math_type T;
    typedef typename _mixture::vector_type _vector;

    //: Constructor
    bsta_top_weight_detector(const _detector& d, const T& w=T(0.5))
      : detect(d), weight_thresh(w) {}

    //: The main function
    bool operator() (const _mixture& mix, const _vector& sample, bool& result) const
    {
      T total_weight = T(0);
      result = false;
      for(unsigned int i=0; i<mix.num_components(); ++i){
        if(total_weight > weight_thresh)
          return true;
        if( !detect(mix.distribution(i),sample,result) )
          return false;
        if(result)
          return true;
        total_weight += mix.weight(i);
      }
      return true;
    }

    //: The detector to apply to components
    _detector detect;
    //: The index to detect
    T weight_thresh;
};
//: Apply a detector to each component in order 
// while the total weight is below a threshold.
// Return true if any tested component matches
template <class _mixture, class _detector>
class bsta_mix_any_less_index_detector
{
  public:
    typedef bool return_T;
    enum { return_dim = 1 };
    typedef typename _mixture::math_type T;
    typedef typename _mixture::vector_type _vector;

    //: Constructor
    bsta_mix_any_less_index_detector(const _detector& d, const T& w=T(0.5))
      : detect(d), weight_thresh(w) {}

    //: The main function
    bool operator() (const _mixture& mix, const _vector& sample, bool& result) const
    {
      T total_weight = T(0);
      result = false;

      bool flag=false;
      for(unsigned int i=0; i<mix.num_components(); ++i){
        if(mix.weight(i) > weight_thresh)
        {
            flag=true;
        if( !detect(mix.distribution(i),sample,result) )
          return false;
        if(result)
          return true;
        }
        //total_weight += mix.weight(i);
      }

      if(flag)
         return false;
      else  
         return true;
    }

    //: The detector to apply to components
    _detector detect;
    //: The index to detect
    T weight_thresh;
};

#endif 
