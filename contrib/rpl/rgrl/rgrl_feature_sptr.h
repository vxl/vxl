
#ifndef rgrl_feature_sptr_h_
#define rgrl_feature_sptr_h_

//:
// \file

#include <vbl/vbl_smart_ptr.h>

class rgrl_feature;

//: Reference-counted smart pointer to rgrl_feature.
typedef vbl_smart_ptr< rgrl_feature > rgrl_feature_sptr;

#endif // rgrl_feature_sptr_h_
