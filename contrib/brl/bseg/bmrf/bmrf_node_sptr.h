// This is brl/bseg/bmrf/bmrf_node_sptr.h
#ifndef bmrf_node_sptr_h_
#define bmrf_node_sptr_h_
//:
// \file
// \brief A smart pointer to a bmrf_node.
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date 1/14/04
//
// Include this file to use the smart pointer bmrf_node_sptr
//

class bmrf_node;

#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<bmrf_node> bmrf_node_sptr;

#endif // bmrf_node_sptr_h_
