#ifndef breg3d_gdbicp_homography_generator_h_
#define breg3d_gdbicp_homography_generator_h_
//:
// \file
// \brief // gdbicp homography generator
//           
// \author Daniel Crispell
// \date 03/01/08
// \verbatim
// Modifications
// 03/25/08 dec  moved to contrib/dec/breg3d
// 08/09/10 jlm  moved to brl/bseg/bvxm/breg3d
// \endverbatim

#include <vcl_string.h>
#include <vimt/vimt_transform_2d.h>

#include "breg3d_homography_generator.h"


class breg3d_gdbicp_homography_generator : public breg3d_homography_generator
{
public:
  breg3d_gdbicp_homography_generator(){};
  
  virtual vimt_transform_2d compute_homography();

private:
  vimt_transform_2d parse_gdbicp_output(vcl_string filename);


};




#endif
