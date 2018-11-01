// This is oxl/osl/osl_edge.cxx
//:
// \file
// \author fsm

#include <iostream>
#include <list>
#include <vector>
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
  v1 = nullptr;
  v2 = nullptr;
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

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
OSL_TOPOLOGY_REF_UNREF_INSTANTIATE(std::list<osl_edge*>);
OSL_TOPOLOGY_REF_UNREF_INSTANTIATE(std::vector<osl_edge*>);
