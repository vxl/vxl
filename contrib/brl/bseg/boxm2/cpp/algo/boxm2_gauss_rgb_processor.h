// This is brl/bseg/boxm2/cpp/algo/boxm2_gauss_rgb_processor.h
#ifndef boxm2_gauss_rgb_processor_h_
#define boxm2_gauss_rgb_processor_h_
//:
// \file
// \brief A class for a GAUSS-RGB processor.
//
// \author Ali Osman Ulusoy
// \date   Feb 22, 2012
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <vector>
#include <vnl/vnl_vector_fixed.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class  boxm2_gauss_rgb_processor
{
 public:
     static vnl_vector_fixed<float,3>  expected_color( vnl_vector_fixed<unsigned char, 8>  mog);

};

#endif // boxm2_gauss_rgb_processor_h_
