// This is oxl/osl/osl_vertex.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm

#include "osl_vertex.h"

#include <osl/osl_hacks.h>

//--------------------------------------------------------------------------------

osl_vertex::osl_vertex(float x_, float y_, int id_) : x(x_), y(y_) {
  SetId(id_);
#if ALLOW_CORELEAKS
  ref();
#endif
}

osl_vertex::~osl_vertex() { }

bool operator==(osl_vertex const &a, osl_vertex const &b) {
  return a.x==b.x && a.y==b.y && a.id==b.id;
}

#include <vcl_compiler.h>
#include <iostream>
#include <list>
#include <vector>
OSL_TOPOLOGY_REF_UNREF_INSTANTIATE(std::list<osl_vertex*>);
OSL_TOPOLOGY_REF_UNREF_INSTANTIATE(std::vector<osl_vertex*>);
