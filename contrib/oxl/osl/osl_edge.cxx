// This is oxl/osl/osl_edge.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk

#include "osl_edge.h"

#include <osl/osl_hacks.h>

//--------------------------------------------------------------------------------

osl_edge::osl_edge(int n, osl_vertex *a, osl_vertex *b)
  : osl_edgel_chain(n)
  , v1(a)
  , v2(b)
{ 
#if ALLOW_CORELEAKS
  ref();
#endif
  v1->ref();
  v2->ref();
}

osl_edge::osl_edge(osl_edgel_chain const &ec, osl_vertex *a, osl_vertex *b)
  : osl_edgel_chain(ec)
  , v1(a)
  , v2(b)
{
#if ALLOW_CORELEAKS
  ref();
#endif
  v1->ref();
  v2->ref();
}

osl_edge::~osl_edge() { 
#if !ALLOW_CORELEAKS
  v1->unref();
  v2->unref();
#endif
  v1 = 0;
  v2 = 0;
}

void osl_edge::set_v1(osl_vertex *newv) {
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
void osl_edge::set_v2(osl_vertex *newv) {
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

float osl_edge::GetStartX() const { return v1->x; }
float osl_edge::GetStartY() const { return v1->y; }

float osl_edge::GetEndX() const { return v2->x; }
float osl_edge::GetEndY() const { return v2->y; }

void osl_edge::SetStartX(float v) { v1->x = v; }
void osl_edge::SetStartY(float v) { v1->y = v; }

void osl_edge::SetEndX(float v) { v2->x = v; }
void osl_edge::SetEndY(float v) { v2->y = v; }

//--------------------------------------------------------------------------------

#include <vcl_list.h>
#include <vcl_vector.h>
OSL_TOPOLOGY_REF_UNREF_INSTANTIATE(vcl_list<osl_edge*>);
OSL_TOPOLOGY_REF_UNREF_INSTANTIATE(vcl_vector<osl_edge*>);
