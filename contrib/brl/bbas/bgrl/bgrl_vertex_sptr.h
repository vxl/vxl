// This is bbas/bbas/bgrl/bgrl_vertex_sptr.h
#ifndef bgrl_vertex_sptr_h_
#define bgrl_vertex_sptr_h_
//:
// \file
// \brief A smart pointer to a bgrl_vertex.
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date 3/17/04
//
// Include this file to use the smart pointer bgrl_vertex_sptr
//

class bgrl_vertex;

#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<bgrl_vertex> bgrl_vertex_sptr;

#endif // bgrl_vertex_sptr_h_
