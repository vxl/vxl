#include "vbl_test_smart_ptr.h"

int base_impl::reftotal = 0;
// Remember, the template instances must be in 
// Templates, but they use this static, so the
// static must be in the library even if only
// one test uses it.

