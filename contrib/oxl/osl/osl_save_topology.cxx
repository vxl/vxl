// This is oxl/osl/osl_save_topology.cxx
//:
// \file
// \author fsm

#include <iostream>
#include <fstream>
#include "osl_save_topology.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

void osl_save_topology(std::ostream &f, std::list<osl_edge*> const &es, std::list<osl_vertex*> const &vs)
{
  unsigned char* vid = nullptr;
  char const *name = "fred";

  // list of all vertices
  std::list<osl_vertex*> stashed;

  // first the explicit vertices
  for (auto v : vs) {
    v->stash_add(name, (void*)++vid);
    stashed.push_front(v);
  }

  // then the edge vertices
  for (auto e : es) {
    if (! e->GetV1()->stash_retrieve(name)) {
      e->GetV1()->stash_add(name, (void*)++vid);
      stashed.push_front(e->GetV1());
    }
    if (! e->GetV2()->stash_retrieve(name)) {
      e->GetV2()->stash_add(name, (void*)++vid);
      stashed.push_front(e->GetV2());
    }
  }

  // version string
  f << "osl_save_topology 1.0\n\n";

  // write the vertices :
  f << stashed.size() << " vertices\n";
  for (auto & i : stashed) {
    void* stashid = (void*) i->stash_retrieve(name);
    f << stashid << ' ' << i->GetId() << ' ' << i->GetX() << ' ' << i->GetY() << std::endl;
  }
  f << std::endl;

  // write the edges :
  f << es.size() << " edges\n";
  for (auto e : es) {
    void* stashid1 = (void*) e->GetV1()->stash_retrieve(name);
    void* stashid2 = (void*) e->GetV2()->stash_retrieve(name);
    f << stashid1 << ' ' << stashid2 << std::endl; // endpoints
    f << e->GetId() << std::endl; // id of edge
    e->osl_edgel_chain::write_ascii(f);
  }

  // remove the stashes :
  for (auto & i : stashed)
    i->stash_remove(name);

  // done
}

void osl_save_topology(char const *f, std::list<osl_edge*> const &e, std::list<osl_vertex*> const &v)
{
  std::ofstream file(f);
  osl_save_topology(file, e, v);
}

void osl_save_topology(char const *f, std::list<osl_edge*> const &e)
{
  osl_save_topology(f, e, std::list<osl_vertex*>());
}

void osl_save_topology(std::ostream &s, std::list<osl_edge*> const &e)
{
  osl_save_topology(s, e, std::list<osl_vertex*>());
}
