/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vsl_save_topology.h"
#include <vcl_fstream.h>

void vsl_save_topology(vcl_ostream &f, vcl_list<vsl_edge*> const &es, vcl_list<vsl_vertex*> const &vs) 
{
  unsigned vid = 0;
  char const *name = "fred";

  // list of all vertices
  vcl_list<vsl_vertex*> stashed;

  // first the explicit vertices
  for (vcl_list<vsl_vertex*>::const_iterator i=vs.begin(); i!=vs.end(); ++i) {
    (*i)->stash_add(name, (void*)++vid);
    stashed.push_front(*i);
  }

  // then the edge vertices
  for (vcl_list<vsl_edge*>::const_iterator i=es.begin(); i!=es.end(); ++i) {
    vsl_edge *e = *i;
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
  f << "vsl_save_topology 1.0" << vcl_endl
    << vcl_endl;
  
  // write the vertices :
  f << stashed.size() << " vertices" << vcl_endl;
  for (vcl_list<vsl_vertex*>::iterator i=stashed.begin(); i!=stashed.end(); ++i) {
    int stashid = (int) (*i)->stash_retrieve(name);
    f << stashid << ' ' << (*i)->GetId() << ' ' << (*i)->GetX() << ' ' << (*i)->GetY() << vcl_endl;
  }
  f << vcl_endl;

  // write the edges :
  f << es.size() << " edges" << vcl_endl;
  for (vcl_list<vsl_edge*>::const_iterator i=es.begin(); i!=es.end(); ++i) {
    int stashid1 = (int) (*i)->GetV1()->stash_retrieve(name);
    int stashid2 = (int) (*i)->GetV2()->stash_retrieve(name);
    f << stashid1 << ' ' << stashid2 << vcl_endl; // endpoints
    f << (*i)->GetId() << vcl_endl; // id of edge
    (*i)->vsl_edgel_chain::write_ascii(f);
  }

  // remove the stashes :
  for (vcl_list<vsl_vertex*>::iterator i=stashed.begin(); i!=stashed.end(); ++i)
    (*i)->stash_remove(name);
  
  // done
}

void vsl_save_topology(char const *f, vcl_list<vsl_edge*> const &e, vcl_list<vsl_vertex*> const &v) 
{
  vcl_ofstream file(f);
  vsl_save_topology(file, e, v);
}

void vsl_save_topology(char const *f, vcl_list<vsl_edge*> const &e)
{
  vsl_save_topology(f, e, vcl_list<vsl_vertex*>());
}

void vsl_save_topology(vcl_ostream &s, vcl_list<vsl_edge*> const &e)
{
  vsl_save_topology(s, e, vcl_list<vsl_vertex*>());
}
