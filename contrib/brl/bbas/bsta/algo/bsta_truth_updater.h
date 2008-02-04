// This is brl/bbas/bsta/algo/bsta_truth_updater.h
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
//   (none yet)
// \endverbatim

#include <bsta/bsta_mixture.h>
#include <vnl/vnl_vector_fixed.h>


//: An updater for ground truth mixtures
template <class dist_, class updater_>
class bsta_truth_updater
{
  public:
    typedef typename dist_::math_type T;
    typedef typename dist_::vector_type sub_vector_;
    typedef vnl_vector_fixed<T,dist_::dimension+1> vector_;

    enum {data_dimension = dist_::dimension+1};

    bsta_truth_updater (const dist_& model, const updater_& updater)
    : model_dist_(model), updater_mbr_(updater) {}

    //: The main function
    void operator() ( bsta_mixture<dist_>& mixture, const vector_& sample ) const
    {
      // The actual sample is stored in the first n-1 dimensions
      sub_vector_ data(sample.data_block());

      // Get the index from the last dimension
      unsigned int index = static_cast<unsigned int>(sample[dist_::dimension]);

      while (mixture.num_components() <= index)
        mixture.insert(model_dist_,T(0));

      updater_mbr_(mixture.distribution(index),data);
    }

  protected:

    //: A model for new distributions inserted
    dist_ model_dist_;

    //: The updater applied to the components
    updater_ updater_mbr_;
};

#endif // bsta_truth_updater_h_
