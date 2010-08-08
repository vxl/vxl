// This is vpgl/ihog/ihog_region_sptr.h
#ifndef ihog_region_sptr_h_
#define ihog_region_sptr_h_
//:
// \file
// \brief Smart pointer to a ihog_region
// \author Matthew Leotta, (mleotta@lems.brown.edu)
// \date 4/24/04
//

class ihog_region;

#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<ihog_region> ihog_region_sptr;

#endif // ihog_region_sptr_h_
