// This is oxl/osl/osl_topology.cxx
//:
// \file
// \author fsm

#include <new>
#include <iostream>
#include <cstring>
#include "osl_topology.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
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

osl_topology_base::osl_topology_base() : id(0), stash_head(nullptr) { }
void  osl_topology_base::stash_add(char const *name,
                                   void const *data,
                                   void (*dtor)(void *))
{
  auto *l = new osl_stash_link(name, data, dtor, stash_head);
  stash_head = l;
}

void  osl_topology_base::stash_replace(char const *name,
                                       void const *data,
                                       void (*dtor)(void *))
{
  for (osl_stash_link *l = stash_head; l; l=l->next) {
    if (std::strcmp(l->name, name) == 0) {
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
    if (std::strcmp(l->name, name) == 0)
      return l->data;
  // not found
  return nullptr;
}

void *osl_topology_base::stash_remove(char const *name) {
  for (osl_stash_link *p = nullptr, *l = stash_head; l; p=l, l=p->next) {
    if (std::strcmp(l->name, name) == 0) {
      if (p)
        p->next = l->next;
      else
        stash_head = l->next;
      delete l;
    }
  }
  // not found
  return nullptr;
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
