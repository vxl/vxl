// This is oxl/osl/osl_save_topology.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk

#include "osl_save_topology.h"
#include <vcl_fstream.h>

void osl_save_topology(vcl_ostream &f, vcl_list<osl_edge*> const &es, vcl_list<osl_vertex*> const &vs)
{
  unsigned vid = 0;
  char const *name = "fred";

  // list of all vertices
  vcl_list<osl_vertex*> stashed;

  // first the explicit vertices
  for (vcl_list<osl_vertex*>::const_iterator i=vs.begin(); i!=vs.end(); ++i) {
    (*i)->stash_add(name, (void*)++vid);
    stashed.push_front(*i);
  }

  // then the edge vertices
  for (vcl_list<osl_edge*>::const_iterator i=es.begin(); i!=es.end(); ++i) {
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
  for (vcl_list<osl_vertex*>::iterator i=stashed.begin(); i!=stashed.end(); ++i) {
    int stashid = (int) (*i)->stash_retrieve(name);
    f << stashid << ' ' << (*i)->GetId() << ' ' << (*i)->GetX() << ' ' << (*i)->GetY() << vcl_endl;
  }
  f << vcl_endl;

  // write the edges :
  f << es.size() << " edges\n";
  for (vcl_list<osl_edge*>::const_iterator i=es.begin(); i!=es.end(); ++i) {
    int stashid1 = (int) (*i)->GetV1()->stash_retrieve(name);
    int stashid2 = (int) (*i)->GetV2()->stash_retrieve(name);
    f << stashid1 << ' ' << stashid2 << vcl_endl; // endpoints
    f << (*i)->GetId() << vcl_endl; // id of edge
    (*i)->osl_edgel_chain::write_ascii(f);
  }

  // remove the stashes :
  for (vcl_list<osl_vertex*>::iterator i=stashed.begin(); i!=stashed.end(); ++i)
    (*i)->stash_remove(name);

  // done
}

void osl_save_topology(char const *f, vcl_list<osl_edge*> const &e, vcl_list<osl_vertex*> const &v)
{
  vcl_ofstream file(f);
  osl_save_topology(file, e, v);
}

void osl_save_topology(char const *f, vcl_list<osl_edge*> const &e)
{
  osl_save_topology(f, e, vcl_list<osl_vertex*>());
}

void osl_save_topology(vcl_ostream &s, vcl_list<osl_edge*> const &e)
{
  osl_save_topology(s, e, vcl_list<osl_vertex*>());
}
