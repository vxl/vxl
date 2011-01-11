// This is brl/bseg/boxm2/cpp/boxm2_mog3_grey_processor.h
#ifndef boxm2_mog3_grey_processor_h_
#define boxm2_mog3_grey_processor_h_
//:
// \file
// \brief A class for a grey-scale-mixture-of-gaussian processor
//
// \author Vishal Jain
// \date   Dec 27, 2010
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vnl/vnl_vector_fixed.h>

class  boxm2_mog3_grey_processor
{
 public:
  static float expected_color( vnl_vector_fixed<unsigned char, 8> mog3);
  static float prob_density( const vnl_vector_fixed<unsigned char, 8> & mog3, float x);
  static float gauss_prob_density(float x, float mu, float sigma);
};

#endif // boxm2_mog3_grey_processor_h_
