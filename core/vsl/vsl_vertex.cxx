/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation "vsl_vertex"
#endif
#include "vsl_vertex.h"

#include <vsl/vsl_hacks.h>

//--------------------------------------------------------------------------------

vsl_vertex::vsl_vertex(float x_, float y_, int id_) : x(x_), y(y_) { 
  SetId(id_); 
#if ALLOW_CORELEAKS
  ref();
#endif
}

vsl_vertex::~vsl_vertex() { }

bool operator==(vsl_vertex const &a, vsl_vertex const &b) {
  return a.x==b.x && a.y==b.y && a.id==b.id;
}

#include <vcl/vcl_list.h>
#include <vcl/vcl_vector.h>
VSL_TOPOLOGY_REF_UNREF_INSTANTIATE(vcl_list<vsl_vertex*>);
VSL_TOPOLOGY_REF_UNREF_INSTANTIATE(vcl_vector<vsl_vertex*>);
