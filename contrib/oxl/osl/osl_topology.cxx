// This is oxl/osl/osl_topology.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm

#include "osl_topology.h"
#include <vcl_new.h>
#include <vcl_cstring.h>
#include <osl/osl_hacks.h>

// Set this to 1 if you think it can avoid heap corruption, and
// you don't mind leaking some core.
#define ALLOW_CORELEAKS 0

//#define fsm_pad +50
#define fsm_pad /* */

//--------------------------------------------------------------------------------

struct osl_stash_link {
  osl_stash_link(char const *name_, void const *data_, void (*dtor_)(void*), osl_stash_link *next_)
    : name(name_), data(const_cast<void*>(data_)), dtor(dtor_), next(next_) { }
  char const *name;
  void *data;
  void (*dtor)(void *);
  osl_stash_link *next;
};

osl_topology_base::osl_topology_base() : id(0), stash_head(0) { }
void  osl_topology_base::stash_add(char const *name,
                                   void const *data,
                                   void (*dtor)(void *))
{
  osl_stash_link *l = new osl_stash_link(name, data, dtor, stash_head);
  stash_head = l;
}

void  osl_topology_base::stash_replace(char const *name,
                                       void const *data,
                                       void (*dtor)(void *))
{
  for (osl_stash_link *l = stash_head; l; l=l->next) {
    if (vcl_strcmp(l->name, name) == 0) {
      l->name = name;
      l->data = const_cast<void*>(data);
      l->dtor = dtor;
      return;
    }
  }
  // not found.
  stash_add(name, data, dtor);
}

void *osl_topology_base::stash_retrieve(char const *name) const {
  for (osl_stash_link *l = stash_head; l; l=l->next)
    if (vcl_strcmp(l->name, name) == 0)
      return l->data;
  // not found
  return 0;
}

void *osl_topology_base::stash_remove(char const *name) {
  for (osl_stash_link *p = 0, *l = stash_head; l; p=l, l=p->next) {
    if (vcl_strcmp(l->name, name) == 0) {
      if (p)
        p->next = l->next;
      else
        stash_head = l->next;
      delete l;
    }
  }
  // not found
  return 0;
}

osl_topology_base::~osl_topology_base() {
  while (stash_head) {
    if (stash_head->dtor) // call dtor?
      (stash_head->dtor)(stash_head->data);
    stash_remove(stash_head->name);
  }
}

void osl_topology_base::SetId(int v) { id = v; }

int osl_topology_base::GetId() const { return id; }

//--------------------------------------------------------------------------------
