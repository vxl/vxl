#ifndef osl_topology_h_
#define osl_topology_h_
#ifdef __GNUC__
#pragma interface
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk
//
// *** This is a permanent conversion hack ***
// Do not expect that future Canny's will provide output in this form.
// Do not expect this code to be supported in any way whatsoever.

#include <vcl_list.h>
#include <vbl/vbl_ref_count.h>

template <class T>
inline T fsm_pop(vcl_list<T> *l)
{
  T tmp = l->front();
  l->pop_front();
  return tmp;
}

//--------------------------------------------------------------------------------

struct osl_stash_link;

struct osl_topology_base : public vbl_ref_count
{
  int id;
  osl_topology_base();
  ~osl_topology_base();
  void SetId(int );
  int GetId() const;

  // add another stash under that name.
  void  stash_add     (char const *name, void const *data, void (*dtor)(void *) = 0);
  // replace first stash ith given name. the old dtor is *not* called.
  void  stash_replace (char const *name, void const *data, void (*dtor)(void *) = 0);
  // return first stash with given name, 0 if none.
  void *stash_retrieve(char const *name) const;
  // remove first stash with given name. the dtor is *not* called.
  void *stash_remove  (char const *name);
private:
  osl_stash_link *stash_head;
};

//--------------------------------------------------------------------------------

//: call ref() on every object pointed to by an element of C.
template <class Container>
inline
void osl_topology_ref(Container &C)
{
  for (typename Container::iterator i=C.begin(); i!=C.end(); ++i)
    if (*i)
      (*i)->ref();
}

//: call unref() on every object pointed to by an element of C.
template <class Container>
inline
void osl_topology_unref(Container &C)
{
  for (typename Container::iterator i=C.begin(); i!=C.end(); ++i)
    if (*i)
      (*i)->unref();
}

#define OSL_TOPOLOGY_REF_UNREF_INSTANTIATE(C) \
VCL_INSTANTIATE_INLINE(void osl_topology_ref(C &)); \
VCL_INSTANTIATE_INLINE(void osl_topology_unref(C &))

#endif // osl_topology_h_
