// This is brl/bbas/bsta/algo/bsta_bayes_functor.h
#ifndef bsta_bayes_functor_h_
#define bsta_bayes_functor_h_
//:
// \file
// \brief Functors for Bayesian classification
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date July 27, 2005
//
// \verbatim
//  Modifications
//   (none yet)
// \endverbatim

#include <vcl_limits.h>


//: A functor that classifies a sample as one of the components of the mixture
template <class comp_dist_>
class bsta_bayes_functor
{
  private:
    typedef bsta_num_obs<comp_dist_> comp_obs_dist_;
    typedef bsta_mixture<comp_obs_dist_> mix_dist_;
    typedef typename comp_dist_::math_type T;
    typedef vnl_vector_fixed<T,comp_dist_::dimension> vector_;

  public:
    typedef int return_T;
    enum { return_dim = 1 };

    //: The main function
    bool operator() ( const mix_dist_& mix,
                      const vector_& sample,
                      return_T& best_index ) const
    {
      best_index = -1;
      T best_probability = T(0);
      for (unsigned int i=0; i<mix.num_components(); ++i){
        T weight = mix.weight(i);
        if (weight > best_probability){
          T prob = mix.distribution(i).prob_density(sample) * weight;
          if (prob > best_probability){
            best_index = i;
            best_probability = prob;
          }
        }
      }
      return best_index >= 0;
    }
};


//: A functor that computes the probability of each component in a mixture
template <class comp_dist_>
class bsta_mixture_prob_functor
{
  private:
    typedef bsta_num_obs<comp_dist_> comp_obs_dist_;
    typedef bsta_mixture<comp_obs_dist_> mix_dist_;
    typedef typename comp_dist_::math_type T;
    typedef vnl_vector_fixed<T,comp_dist_::dimension> vector_;

  public:
    typedef vector_ return_T;
    enum { return_dim = comp_dist_::dimension };

    bsta_mixture_prob_functor(bool normalize = true, unsigned int num_cmps=3)
    : num_cmps_(num_cmps), normalize_(normalize) {}

    //: The main function
    bool operator() ( const mix_dist_& mix,
                      const vector_& sample,
                      return_T& result ) const
    {
      result = return_T();
      if (num_cmps_==0)
        return false;

      double tmp = 0.0;
      for (unsigned int i=0; i<mix.num_components(); ++i){
        T w = mix.weight(i);
        if (w > T(0)){
          result[i] = w * mix.distribution(i).prob_density(sample);
          tmp += result[i];
        }
      }
      if (normalize_){
        if (tmp > vcl_numeric_limits<T>::epsilon()){
          for (unsigned int i=0; i<mix.num_components(); ++i){
            result[i] /= tmp;
          }
        }
        else{
          for (unsigned int i=0; i<mix.num_components(); ++i)
            result[i] = mix.weight(i);
        }
      }
      return true;
    }

  protected:
    unsigned int num_cmps_;
    bool normalize_;
};


#endif // bsta_bayes_functor_h_
