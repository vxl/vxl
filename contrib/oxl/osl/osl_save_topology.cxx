// This is oxl/osl/osl_save_topology.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm

#include <iostream>
#include <fstream>
#include "osl_save_topology.h"
#include <vcl_compiler.h>

void osl_save_topology(std::ostream &f, std::list<osl_edge*> const &es, std::list<osl_vertex*> const &vs)
{
  unsigned char* vid = nullptr;
  char const *name = "fred";

  // list of all vertices
  std::list<osl_vertex*> stashed;

  // first the explicit vertices
  for (std::list<osl_vertex*>::const_iterator i=vs.begin(); i!=vs.end(); ++i) {
    (*i)->stash_add(name, (void*)++vid);
    stashed.push_front(*i);
  }

  // then the edge vertices
  for (std::list<osl_edge*>::const_iterator i=es.begin(); i!=es.end(); ++i) {
    osl_edge *e = *i;
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
  for (std::list<osl_vertex*>::iterator i=stashed.begin(); i!=stashed.end(); ++i) {
    void* stashid = (void*) (*i)->stash_retrieve(name);
    f << stashid << ' ' << (*i)->GetId() << ' ' << (*i)->GetX() << ' ' << (*i)->GetY() << std::endl;
  }
  f << std::endl;

  // write the edges :
  f << es.size() << " edges\n";
  for (std::list<osl_edge*>::const_iterator i=es.begin(); i!=es.end(); ++i) {
    void* stashid1 = (void*) (*i)->GetV1()->stash_retrieve(name);
    void* stashid2 = (void*) (*i)->GetV2()->stash_retrieve(name);
    f << stashid1 << ' ' << stashid2 << std::endl; // endpoints
    f << (*i)->GetId() << std::endl; // id of edge
    (*i)->osl_edgel_chain::write_ascii(f);
  }

  // remove the stashes :
  for (std::list<osl_vertex*>::iterator i=stashed.begin(); i!=stashed.end(); ++i)
    (*i)->stash_remove(name);

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
