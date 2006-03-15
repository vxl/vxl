#ifndef rgrl_internal_util_h_
#define rgrl_internal_util_h_
//:
// \file
// \author Gehua Yang
// \date   March 2006
// \brief  Place to put some utility functions used in multiple places
//
// \verbatim
//  Modifications:
// \endverbatim
//

#include <vnl/vnl_fwd.h>
#include <rgrl/rgrl_fwd.h>

bool
rgrl_internal_util_upgrade_to_homography2D( vnl_matrix_fixed<double, 3, 3>& init_H,
                                            rgrl_transformation const& cur_transform );

#endif // rgrl_internal_util_h_
