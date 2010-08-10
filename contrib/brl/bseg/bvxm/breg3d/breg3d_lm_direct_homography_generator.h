#ifndef breg3d_lm_direct_homography_generator_h_
#define breg3d_lm_direct_homography_generator_h_
//:
// \file
// \brief // levenberg marquardt direct homography generator
//           
// \author Daniel Crispell
// \date 03/01/08
// \verbatim
// Modifications
// 03/25/08 dec  moved to contrib/dec/breg3d
// 08/09/10 jlm  moved to brl/bseg/bvxm/breg3d
// \endverbatim

#include <vimt/vimt_transform_2d.h>

#include "breg3d_homography_generator.h"


class breg3d_lm_direct_homography_generator : public breg3d_homography_generator
{
public:
  breg3d_lm_direct_homography_generator(){};
  
  virtual vimt_transform_2d compute_homography();


};




#endif
