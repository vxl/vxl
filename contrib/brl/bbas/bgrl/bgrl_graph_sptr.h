// This is brl/bbas/bgrl/bgrl_graph_sptr.h
#ifndef bgrl_graph_sptr_h_
#define bgrl_graph_sptr_h_
//:
// \file
// \brief A smart pointer to a bgrl_graph.
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date 3/17/04
//
// Include this file to use the smart pointer bgrl_graph_sptr
//

class bgrl_graph;

#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<bgrl_graph> bgrl_graph_sptr;

#endif // bgrl_graph_sptr_h_
