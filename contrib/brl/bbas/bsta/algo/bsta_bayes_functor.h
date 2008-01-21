// This is brcv/seg/bsta/bsta_bayes_functor.h
#ifndef bsta_bayes_functor_h_
#define bsta_bayes_functor_h_

//:
// \file
// \brief Functors for Bayesian classification 
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 07/27/05
//
// 
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_limits.h>


//: A functor that classifies a sample as one of the components of the mixture
template <class _comp_dist>
class bsta_bayes_functor
{
  private:
    typedef bsta_num_obs<_comp_dist> _comp_obs_dist;
    typedef bsta_mixture<_comp_obs_dist> _mix_dist;
    typedef typename _comp_dist::math_type T;
    typedef vnl_vector_fixed<T,_comp_dist::dimension> _vector;

  public:
    typedef int return_T;
    enum { return_dim = 1 };

    //: The main function
    bool operator() ( const _mix_dist& mix,
                      const _vector& sample,
                      return_T& best_index ) const
    {
      best_index = -1;
      T best_probability = T(0);
      for(unsigned int i=0; i<mix.num_components(); ++i){
        T weight = mix.weight(i);
        if(weight > best_probability){
          T prob = mix.distribution(i).prob_density(sample) * weight;
          if(prob > best_probability){
            best_index = i;
            best_probability = prob;
          }
        }
      }
      return (best_index >= 0);
    }

};


//: A functor that computes the probability of each component in a mixture
template <class _comp_dist>
class bsta_mixture_prob_functor
{
  private:
    typedef bsta_num_obs<_comp_dist> _comp_obs_dist;
    typedef bsta_mixture<_comp_obs_dist> _mix_dist;
    typedef typename _comp_dist::math_type T;
    typedef vnl_vector_fixed<T,_comp_dist::dimension> _vector;

  public:
    typedef _vector return_T;
    enum { return_dim = _comp_dist::dimension };

    bsta_mixture_prob_functor(bool normalize = true, unsigned int num_cmps=3)
    : num_cmps_(num_cmps), normalize_(normalize) {}

    //: The main function
    bool operator() ( const _mix_dist& mix,
                      const _vector& sample,
                      return_T& result ) const
    {
      result = return_T();
      if(num_cmps_==0)
        return false;

      double tmp = 0.0;
      for(unsigned int i=0; i<mix.num_components(); ++i){
        T w = mix.weight(i);
        if(w > T(0)){
          result[i] = w * mix.distribution(i).prob_density(sample);
          tmp += result[i];
        }
      }
      if(normalize_){
        if(tmp > vcl_numeric_limits<T>::epsilon()){
          for(unsigned int i=0; i<mix.num_components(); ++i){
            result[i] /= tmp;
          }
        }
        else{
          for(unsigned int i=0; i<mix.num_components(); ++i)
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
