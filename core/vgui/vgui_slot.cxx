// This is oxl/vgui/vgui_slot.cxx
#include "vgui_slot.h"
//:
// \file
// \author fsm@robots.ox.ac.uk
// \brief  See vgui_slot.h for a description of this file.
//
// Implementation notes:
//
// [1]
// Since tableaux will hold slots and use them to refer to children, each
// slot must ref()erence its child.
//
// [2]
// A slot should not ref()erence its parent as that would lead to cyclic
// dependencies and hence core leaks. Thus, we use a raw pointer to hold
// the parent.
//
// [3]
// A vgui_tableau_sptr could be used to hold the child, but there is no real
// advantage in that.
//
// [4]
// A slot's parent pointer must never be zero because a slot which does not
// have a parent is a useless slot (and so is most likely an error). Thus
// 'p' is a private data member of vgui_slot_impl and the constructor will
// cause assertion failure if given a null parent pointer.
//
// [5]
// A slot's parent pointer cannot be changed because there is no legitimite
// use for that.

#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_set.h>

#include <vgui/vgui_event.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_macro.h>

//:
// For efficiency (e.g. when posting redraws), the parents of a tableau
// may be cached in the vgui_slot_data baseclass of vgui_tableau. This
// macro enables that optimization. If caching is not enabled, parents
// are computed by scanning the entire registry of slots in existence
// which could obviously be quite slow.
#define cache_parents 1

//:
// this container holds a single pointer for every impl object.
// the pointers are cast to and from void* to avoid (a) exposing the
// implementation class and (b) instantiating an extra class template.

struct vgui_slot_impl
{
  // all is stored as a pointer as it must live longer than any static slots
  typedef vcl_set<void *> all_t;
  static all_t* all;
  static int all_refs;

  inline vgui_slot_impl(vgui_tableau *p_, vgui_tableau *c_);
  inline ~vgui_slot_impl();

  // this changes the child, not the parent.
  inline void assign(vgui_tableau *t);

  // there's nothing tricky here. we just return the raw pointers.
  vgui_tableau *parent() const { return p; }
  vgui_tableau *child () const { return c; }

  // reference counting. deriving from vbl_ref_count would make the
  // methods virtual and hence non-inlineable.
  inline void acquire();
  inline void release();

private:
  vgui_tableau *p; // parent
  vgui_tableau *c; // child
  int use_count;   // reference count

  // helpers. the purpose of these functions is to perform
  // the double link/unlink required to cache the parent-child
  // relation on tableaux. they are static because they may be
  // invoked when the impl object is in a dubious state.
  static inline void link  (vgui_tableau *p, vgui_tableau *c);
  static inline void unlink(vgui_tableau *p, vgui_tableau *c);
};

// static data for impl class :
vcl_set<void*>* vgui_slot_impl::all = 0;
int vgui_slot_impl::all_refs = -1;

void vgui_slot_impl::acquire()
{
  ++ use_count;
}

void vgui_slot_impl::release()
{
  assert(use_count > 0);

  if (-- use_count == 0)
    delete this;
}

void vgui_slot_impl::unlink(vgui_tableau *p, vgui_tableau *c)
{
#if cache_parents
  if (c) {
    vcl_vector<vgui_tableau*> &vec = c->vgui_slot_data::parents;
    for (vcl_vector<vgui_tableau*>::iterator i=vec.begin(); i!=vec.end(); ++i) {
      if (*i == p) {
        vec.erase(i);
        break;
      }
    }
  }
#endif
}

void vgui_slot_impl::link(vgui_tableau *p, vgui_tableau *c)
{
#if cache_parents
  if (c)
    c->vgui_slot_data::parents.push_back(p);
#endif
}

vgui_slot_impl::vgui_slot_impl(vgui_tableau *p_, vgui_tableau *c_)
  : p(p_)
  , c(c_)
  , use_count(0)
{
  if (! p) {
    vgui_macro_warning << "parent is null" << vcl_endl;
    assert(false);
  }

  if (c)
    c->ref();

  // register.
  if (all == 0) {
    //vcl_cerr << __FILE__ ": CREATING slot cache\n";
    all = new all_t;
    all_refs = 0;
  }
  ++all_refs;
  all->insert(this);

  // parent and child are not allowed to be equal.
  if (p == c) {
    vgui_macro_warning << "parent and child are equal" << vcl_endl;
    assert(false);
  }

  link(p, c);
}

vgui_slot_impl::~vgui_slot_impl()
{
  unlink(p, c);

  if (c)
    c->unref();

  // deregister.
  vcl_set<void*>::iterator i = all->find(this);
  assert(i != all->end());
  all->erase(i);
  if (--all_refs == 0) {
    //vcl_cerr << __FILE__ ": DELETING slot cache\n";
    delete all;
    all = 0;
  }
}

void vgui_slot_impl::assign(vgui_tableau *t)
{
  if (t == c)
    return;

  if (t == p) {
    vgui_macro_warning << "cannot assign() a slot\'s parent to its child" << vcl_endl;
    assert(false);
  }

  unlink(p, c);

  if (t)
    t->ref();

  // remember the old 'c' so that it can be unref()fed below.
  // unreffing it here might cause '*this' to be deleted which
  // would be disastrous because we're about to assign to 'this->c'.
  vgui_tableau *old_c = c;

  c = t;

  link(p, c);

  // it's safe(r) to unref() now.
  if (old_c)
    old_c->unref();
}

//--------------------------------------------------------------------------------

vgui_slot::vgui_slot() : pimpl(0) { }

vgui_slot::vgui_slot(vgui_tableau *p)
{
  pimpl = new vgui_slot_impl(p, 0);
  pimpl->acquire();
}

vgui_slot::vgui_slot(vgui_tableau *p, vgui_tableau_sptr const &c)
{
  pimpl = new vgui_slot_impl(p, c.operator->());
  pimpl->acquire();
}

vgui_slot::vgui_slot(vgui_slot const &that)
{
  pimpl = that.pimpl;

  if (pimpl)
    pimpl->acquire();
}

vgui_slot::~vgui_slot()
{
  if (pimpl)
    pimpl->release();

  pimpl = 0;
}

vgui_slot &vgui_slot::operator=(vgui_slot const &that)
{
  if (pimpl != that.pimpl) {
    if (that.pimpl)
      that.pimpl->acquire();

    if (pimpl)
      pimpl->release();

    pimpl = that.pimpl;
  }

  return *this;
}

vgui_tableau_sptr vgui_slot::parent() const
{
  return pimpl ? pimpl->parent() : 0;
}

vgui_tableau_sptr vgui_slot::child()  const
{
  return pimpl ? pimpl->child () : 0;
}

bool vgui_slot::operator==(vgui_tableau_sptr const &t) const
{
  return child() == t;
}

void vgui_slot::assign(vgui_tableau_sptr const &t)
{
  if (pimpl)
    pimpl->assign(t.operator->());
  else {
    vgui_macro_warning << "attempted assign() to empty slot." << vcl_endl;
    vgui_macro_warning << "t = " << t << vcl_endl;
    assert(false);
  }
}

bool vgui_slot::handle(vgui_event const &e)
{
  if (!pimpl) return false;
  vgui_tableau* c = pimpl->child();
  if (!c) return false;

  return c->handle(e);
}

vgui_slot::operator bool() const
{
  return pimpl && (pimpl->child() != 0);
}

vgui_slot::operator vgui_tableau_sptr() const
{
  return pimpl ? pimpl->child() : 0;
}

vgui_tableau *vgui_slot::operator->() const
{
  return pimpl ? pimpl->child() : 0;
}

vcl_ostream & operator<<(vcl_ostream &os, vgui_slot const &s)
{
  // the reason for the flush() is to get as much stuff as
  // possible printed before an eventual segfault.
  return os << "vgui_slot("
            << vcl_flush
            << static_cast<void*>( s.parent().operator->() ) << ", "
            << vcl_flush
            << static_cast<void*>( s.child ().operator->() ) << ")"
            << vcl_flush;
}

//--------------------------------------------------------------------------------

void vgui_slot::get_children_of(vgui_tableau_sptr const& tab, vcl_vector<vgui_tableau_sptr> *children)
{
  for (vcl_set<void*>::iterator i=vgui_slot_impl::all->begin(); i!=vgui_slot_impl::all->end(); ++i) {
    vgui_slot_impl *ptr = static_cast<vgui_slot_impl*>(*i);
    if ( ptr->parent() == tab.operator->() )
      children->push_back( ptr->child() );
  }
}

void vgui_slot::get_parents_of (vgui_tableau_sptr const& tab, vcl_vector<vgui_tableau_sptr> *parents)
{
#if cache_parents
  vcl_vector<vgui_tableau*> const &vec = tab->vgui_slot_data::parents;
  for (unsigned i=0; i<vec.size(); ++i)
    parents->push_back(vec[i]);
#else
  for (vcl_set<void*>::iterator i=vgui_slot_impl::all->begin(); i!=vgui_slot_impl::all->end(); ++i) {
    vgui_slot_impl *ptr = static_cast<vgui_slot_impl*>(*i);
    if ( ptr->child() == tab.operator->() )
      children->push_back( ptr->parent() );
  }
#endif
}

void vgui_slot::replace_child_everywhere (vgui_tableau_sptr const &old_child,
                                          vgui_tableau_sptr const &new_child)
{
  // the default is 'false'. don't check in 'true'.
  static bool debug = false;

  if (debug)
    vcl_cerr << "vgui_slot replace_child_everywhere " << vcl_endl
             << "old_child : " << old_child->pretty_name()
             << "\t"
             << "new child : " << new_child->pretty_name() << vcl_endl;

  if (old_child == new_child)
    vcl_cerr << "vgui_slot::replace_child_everywhere: old_child == new_child\n";

  for (vcl_set<void*>::iterator i=vgui_slot_impl::all->begin(); i!=vgui_slot_impl::all->end(); ++i) {
    vgui_slot_impl *ptr = static_cast<vgui_slot_impl*>(*i);

    if (debug) {
      vcl_cerr << "slot  "
               << "parent : " << ptr->parent()->pretty_name()
               << "\tchild : ";
      if (! ptr->child())
        vcl_cerr << "0" << vcl_endl;
      else
        vcl_cerr << ptr->child()->pretty_name() << vcl_endl;
    }

    if ( ptr->child() == old_child.operator->() ) {
      assert(ptr->parent() != new_child.operator->() );
      if (debug)
        vcl_cerr << "replace: " << ptr->child() << vcl_endl;
      ptr->parent()->notify_replaced_child(old_child, new_child);
      ptr->assign(new_child.operator->());
    }
  }
}

//--------------------------------------------------------------------------------
