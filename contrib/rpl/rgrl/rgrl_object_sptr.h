
#ifndef rgrl_object_sptr_h_
#define rgrl_object_sptr_h_

//:
// \file

#include <vbl/vbl_smart_ptr.h>

class rgrl_object;

//: Reference-counted smart pointer to rgrl_feature.
typedef vbl_smart_ptr< rgrl_object > rgrl_object_sptr;

#endif // rgrl_object_sptr_h_
