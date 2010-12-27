// This is brl/bseg/boxm/sample/algo/boxm2_mog3_grey_processor.h
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


#include <vnl/vnl_random.h>

#include <boxm2/boxm2_data_traits.h>


class  boxm2_mog3_grey_processor
{

 public:
  static float expected_color( vnl_vector_fixed<unsigned char, 8> mog3);
    
};

#endif // boxm2_mog3_grey_processorr_h_
