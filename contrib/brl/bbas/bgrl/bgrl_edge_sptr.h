// This is brl/bbas/bgrl/bgrl_edge_sptr.h
#ifndef bgrl_edge_sptr_h_
#define bgrl_edge_sptr_h_
//:
// \file
// \brief A smart pointer to a bgrl_edge.
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date 3/17/04
//
// Include this file to use the smart pointer bgrl_edge_sptr
//

class bgrl_edge;

#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<bgrl_edge> bgrl_edge_sptr;

#endif // bgrl_edge_sptr_h_
