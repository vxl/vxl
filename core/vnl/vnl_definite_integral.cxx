#include "vnl_definite_integral.h"
#include <vnl/vnl_integrant_fnct.h>

// initial the static memeber
vnl_integrant_fnct* vnl_definite_integral::pfnct_ = 0;
