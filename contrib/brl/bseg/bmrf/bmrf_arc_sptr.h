// This is brl/bseg/bmrf/bmrf_arc_sptr.h
#ifndef bmrf_arc_sptr_h_
#define bmrf_arc_sptr_h_
//:
// \file
// \brief A smart pointer to a bmrf_arc.
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date 6/7/04
//
// Include this file to use the smart pointer bmrf_arc_sptr
//

class bmrf_arc;

#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<bmrf_arc> bmrf_arc_sptr;

#endif // bmrf_arc_sptr_h_
