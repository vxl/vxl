// This is brl/bbas/bgrl/bgrl_search_func_sptr.h
#ifndef bgrl_search_func_sptr_h_
#define bgrl_search_func_sptr_h_
//:
// \file
// \brief A smart pointer to a bgrl_search_func.
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date 3/24/04
//
// Include this file to use the smart pointer bgrl_search_func_sptr
//

class bgrl_search_func;

#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<bgrl_search_func> bgrl_search_func_sptr;

#endif // bgrl_search_func_sptr_h_
