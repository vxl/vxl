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
};

#endif // boxm2_mog3_grey_processor_h_
