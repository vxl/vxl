
#ifndef rgrl_event_sptr_h_
#define rgrl_event_sptr_h_

//:
// \file

#include <vbl/vbl_smart_ptr.h>

class rgrl_event;

//: Reference-counted smart pointer to rgrl_event.
typedef vbl_smart_ptr< rgrl_event > rgrl_event_sptr;

#endif // rgrl_event_sptr_h_
