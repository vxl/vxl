/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vsl_topology.h"
#include <vcl_new.h>
#include <vcl_cstring.h>
#include <vcl_iostream.h>
#include <vcl_iomanip.h>
#include <vsl/vsl_hacks.h>

// Set this to 1 if you think it can avoid heap corruption, and
// you don't mind leaking some core.
#define ALLOW_CORELEAKS 0

//#define fsm_pad +50
#define fsm_pad /* */

//--------------------------------------------------------------------------------

struct vsl_stash_link {
  vsl_stash_link(char const *name_, void const *data_, void (*dtor_)(void*), vsl_stash_link *next_)
    : name(name_), data(const_cast<void*>(data_)), dtor(dtor_), next(next_) { }
  char const *name;
  void *data;
  void (*dtor)(void *);
  vsl_stash_link *next;
};

vsl_topology_base::vsl_topology_base() : id(0), stash_head(0) { }
void  vsl_topology_base::stash_add(char const *name, 
				   void const *data, 
				   void (*dtor)(void *) VCL_DEFAULT_VALUE(0))
{
  vsl_stash_link *l = new vsl_stash_link(name, data, dtor, stash_head);
  stash_head = l;
}

void  vsl_topology_base::stash_replace(char const *name, 
				       void const *data, 
				       void (*dtor)(void *) VCL_DEFAULT_VALUE(0))
{
  for (vsl_stash_link *l = stash_head; l; l=l->next) {
    if (strcmp(l->name, name) == 0) {
      l->name = name;
      l->data = const_cast<void*>(data);
      l->dtor = dtor;
      return;
    }
  }
  // not found.
  stash_add(name, data, dtor);
}

void *vsl_topology_base::stash_retrieve(char const *name) const {
  for (vsl_stash_link *l = stash_head; l; l=l->next)
    if (strcmp(l->name, name) == 0)
      return l->data;
  // not found
  return 0;
}

void *vsl_topology_base::stash_remove(char const *name) {
  for (vsl_stash_link *p = 0, *l = stash_head; l; p=l, l=p->next) {
    if (strcmp(l->name, name) == 0) {
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

vsl_topology_base::~vsl_topology_base() {
  while (stash_head) {
    if (stash_head->dtor) // call dtor?
      (stash_head->dtor)(stash_head->data);
    stash_remove(stash_head->name);
  }
}

void vsl_topology_base::SetId(int v) { id = v; }

int vsl_topology_base::GetId() const { return id; }

//--------------------------------------------------------------------------------
