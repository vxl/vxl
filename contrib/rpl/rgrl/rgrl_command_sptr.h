
#ifndef rgrl_command_sptr_h_
#define rgrl_command_sptr_h_

//:
// \file

#include <vbl/vbl_smart_ptr.h>

class rgrl_command;

//: Reference-counted smart pointer to rgrl_command.
typedef vbl_smart_ptr< rgrl_command > rgrl_command_sptr;

#endif // rgrl_command_sptr_h_
