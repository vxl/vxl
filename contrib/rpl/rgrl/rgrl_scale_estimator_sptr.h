#ifndef rgrl_scale_estimator_sptr_h_
#define rgrl_scale_estimator_sptr_h_

//:
// \file
// \author Amitha Perera
// \date   Feb 2003

#include <vbl/vbl_smart_ptr.h>

class rgrl_scale_estimator_unwgted;
class rgrl_scale_estimator_wgted;

typedef vbl_smart_ptr< rgrl_scale_estimator_wgted > rgrl_scale_estimator_wgted_sptr;
typedef vbl_smart_ptr< rgrl_scale_estimator_unwgted > rgrl_scale_estimator_unwgted_sptr;

#endif // rgrl_scale_estimator_sptr_h_
