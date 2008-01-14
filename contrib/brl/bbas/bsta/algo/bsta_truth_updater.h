// This is brcv/seg/bsta/algo/bsta_truth_updater.h
#ifndef bsta_truth_updater_h_
#define bsta_truth_updater_h_

//:
// \file
// \brief Ground truth mixture updater 
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 07/26/05
//
// \verbatim
//  Modifications
// \endverbatim


#include <bsta/bsta_mixture.h>
#include <vnl/vnl_vector_fixed.h>


//: An updater for ground truth mixtures
template <class _dist, class _updater>
class bsta_truth_updater
{
  public:
    typedef typename _dist::math_type T;
    typedef typename _dist::vector_type _sub_vector;
    typedef vnl_vector_fixed<T,_dist::dimension+1> _vector;

    enum {data_dimension = _dist::dimension+1};

    bsta_truth_updater (const _dist& model, const _updater& updater)
    : model_dist_(model), updater_(updater) {}

    //: The main function
    void operator() ( bsta_mixture<_dist>& mixture, const _vector& sample ) const
    {
      // The actual sample is stored in the first n-1 dimensions
      _sub_vector data(sample.data_block());

      // Get the index from the last dimension
      unsigned int index = static_cast<unsigned int>(sample[_dist::dimension]);

      while(mixture.num_components() <= index)
        mixture.insert(model_dist_,T(0));

      updater_(mixture.distribution(index),data);

    }

  protected:

    //: A model for new distributions inserted
    _dist model_dist_;

    //: The updater applied to the components
    _updater updater_;
};


#endif // bsta_truth_updater_h_
