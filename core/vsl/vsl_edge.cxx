/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vsl_edge.h"

#include <vsl/vsl_hacks.h>

//--------------------------------------------------------------------------------

vsl_edge::vsl_edge(int n, vsl_vertex *a, vsl_vertex *b)
  : vsl_edgel_chain(n)
  , v1(a)
  , v2(b)
{ 
#if ALLOW_CORELEAKS
  ref();
#endif
  v1->ref();
  v2->ref();
}

vsl_edge::vsl_edge(vsl_edgel_chain const &ec, vsl_vertex *a, vsl_vertex *b)
  : vsl_edgel_chain(ec)
  , v1(a)
  , v2(b)
{
#if ALLOW_CORELEAKS
  ref();
#endif
  v1->ref();
  v2->ref();
}

vsl_edge::~vsl_edge() { 
#if !ALLOW_CORELEAKS
  v1->unref();
  v2->unref();
#endif
  v1 = 0;
  v2 = 0;
}

void vsl_edge::set_v1(vsl_vertex *newv) {
  if (v1 == newv)
    return;
  if (newv)
    newv->ref();
#if !ALLOW_CORELEAKS
  if (v1)
    v1->unref();
#endif
  v1 = newv;
}
void vsl_edge::set_v2(vsl_vertex *newv) {
  if (v2 == newv)
    return;
  if (newv)
    newv->ref();
#if !ALLOW_CORELEAKS
  if (v2)
    v2->unref();
#endif
  v2 = newv;
}

float vsl_edge::GetStartX() const { return v1->x; }
float vsl_edge::GetStartY() const { return v1->y; }

float vsl_edge::GetEndX() const { return v2->x; }
float vsl_edge::GetEndY() const { return v2->y; }

void vsl_edge::SetStartX(float v) { v1->x = v; }
void vsl_edge::SetStartY(float v) { v1->y = v; }

void vsl_edge::SetEndX(float v) { v2->x = v; }
void vsl_edge::SetEndY(float v) { v2->y = v; }

//--------------------------------------------------------------------------------

#include <vcl_list.h>
#include <vcl_vector.h>
VSL_TOPOLOGY_REF_UNREF_INSTANTIATE(vcl_list<vsl_edge*>);
VSL_TOPOLOGY_REF_UNREF_INSTANTIATE(vcl_vector<vsl_edge*>);
