#ifndef breg3d_lm_direct_homography_generator_h_
#define breg3d_lm_direct_homography_generator_h_
//:
// \file
// \brief levenberg marquardt direct homography generator
//
// \author Daniel Crispell
// \date Mar 01, 2008
// \verbatim
//  Modifications
//   Mar 25 2008 dec  moved to contrib/dec/breg3d
//   Aug 09 2010 jlm  moved to brl/bseg/bvxm/breg3d
// \endverbatim

#include <ihog/ihog_transform_2d.h>

#include "breg3d_homography_generator.h"


class breg3d_lm_direct_homography_generator : public breg3d_homography_generator
{
 public:
  breg3d_lm_direct_homography_generator()= default;

  ihog_transform_2d compute_homography() override;
};


#endif
