// This is core/vgui/vgui_parent_child_link.cxx
#include "vgui_parent_child_link.h"
//:
// \file
// \author fsm
// \brief  See vgui_parent_child_link.h for a description of this file.

#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_set.h>

#include <vgui/vgui_event.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_macro.h>

// For efficiency (e.g. when posting redraws), the parents of a tableau
// may be cached in the vgui_parent_child_link_data baseclass of vgui_tableau.
// This macro enables that optimization. If caching is not enabled, parents
// are computed by scanning the entire registry of parent_child_links in
// existence which could obviously be quite slow.
#define cache_parents 1

//: Smart-pointer implementation of vgui_parent_child_link.
//
//  This container holds a single pointer for every impl object.
//  The pointers are cast to and from void* to avoid (a) exposing the
//  implementation class and (b) instantiating an extra class template.
//
//  Implementation notes:
//
//  [1]
//  Since tableaux will hold parent_child_links and use them to refer to
//  children, each parent_child_link must ref()erence its child.
//
//  [2]
//  A parent_child_link should not ref()erence its parent as that would lead to
//  cyclic dependencies and hence core leaks. Thus, we use a raw pointer to hold
//  the parent.
//
//  [3]
//  A vgui_tableau_sptr could be used to hold the child, but there is no real
//  advantage in that.
//
//  [4]
//  A parent_child_link's parent pointer must never be zero because a
//  parent_child_link which does not have a parent is a useless
//  parent_child_link (and so is most likely an error). Thus 'p' is a private
//  data member of vgui_parent_child_link_impl and the constructor will
//  cause assertion failure if given a null parent pointer.
//
//  [5]
//  A parent_child_link's parent pointer cannot be changed because there is no
//  legitimate use for that.
struct vgui_parent_child_link_impl
{
  // all is stored as a pointer as it must live longer than any static
  // parent_child_links
  typedef vcl_set<void *> all_t;
  static all_t* all;
  static int all_refs;

  inline vgui_parent_child_link_impl(vgui_tableau *p_, vgui_tableau *c_);
  inline ~vgui_parent_child_link_impl();

  // This changes the child, not the parent.
  inline void assign(vgui_tableau *t);

  // There's nothing tricky here. we just return the raw pointers.
  vgui_tableau *parent() const { return p; }
  vgui_tableau *child () const { return c; }

  // Reference counting. Deriving from vbl_ref_count would make the
  // methods virtual and hence non-inlineable.
  inline void acquire();
  inline void release();

 private:
  vgui_tableau *p; // parent
  vgui_tableau *c; // child
  int use_count;   // reference count

  // Helpers. the purpose of these functions is to perform
  // the double link/unlink required to cache the parent-child
  // relation on tableaux. they are static because they may be
  // invoked when the impl object is in a dubious state.
  static inline void link  (vgui_tableau *p, vgui_tableau *c);
  static inline void unlink(vgui_tableau *p, vgui_tableau *c);
};

// static data for impl class :
vcl_set<void*>* vgui_parent_child_link_impl::all = 0;
int vgui_parent_child_link_impl::all_refs = -1;

void vgui_parent_child_link_impl::acquire()
{
  ++ use_count;
}

void vgui_parent_child_link_impl::release()
{
  assert(use_count > 0);

  if (-- use_count == 0)
    delete this;
}

void vgui_parent_child_link_impl::unlink(vgui_tableau *p, vgui_tableau *c)
{
#if cache_parents
  if (c) {
    vcl_vector<vgui_tableau*> &vec = c->vgui_parent_child_link_data::parents;
    for (vcl_vector<vgui_tableau*>::iterator i=vec.begin(); i!=vec.end(); ++i) {
      if (*i == p) {
        vec.erase(i);
        break;
      }
    }
  }
#endif
}

void vgui_parent_child_link_impl::link(vgui_tableau *p, vgui_tableau *c)
{
#if cache_parents
  if (c)
    c->vgui_parent_child_link_data::parents.push_back(p);
#endif
}

vgui_parent_child_link_impl::vgui_parent_child_link_impl(vgui_tableau *p_, vgui_tableau *c_)
  : p(p_)
  , c(c_)
  , use_count(0)
{
  if (! p) {
    vgui_macro_warning << "parent is null\n";
    assert(false);
  }

  if (c)
    c->ref();

  // register.
  if (all == 0) {
#ifdef DEBUG
    vcl_cerr << __FILE__ " : CREATING parent_child_link cache\n";
#endif
    all = new all_t;
    all_refs = 0;
  }
  ++all_refs;
  all->insert(this);

  // parent and child are not allowed to be equal.
  if (p == c) {
    vgui_macro_warning << "parent and child are equal\n";
    assert(false);
  }

  link(p, c);
}

vgui_parent_child_link_impl::~vgui_parent_child_link_impl()
{
  unlink(p, c);

  if (c)
    c->unref();

  // deregister.
  vcl_set<void*>::iterator i = all->find(this);
  assert(i != all->end());
  all->erase(i);
  if (--all_refs == 0) {
#ifdef DEBUG
    vcl_cerr << __FILE__ " : DELETING parent_child_link cache\n";
#endif
    delete all;
    all = 0;
  }
}

void vgui_parent_child_link_impl::assign(vgui_tableau *t)
{
  if (t == c)
    return;

  if (t == p) {
    vgui_macro_warning << "cannot assign() a parent_child_link\'s parent to its child\n";
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

//------------------------------------------------------------------------------

vgui_parent_child_link::vgui_parent_child_link() : pimpl(0) { }

vgui_parent_child_link::vgui_parent_child_link(vgui_tableau *p)
{
  pimpl = new vgui_parent_child_link_impl(p, 0);
  pimpl->acquire();
}

vgui_parent_child_link::vgui_parent_child_link(vgui_tableau *p,
                                               vgui_tableau_sptr const &c)
{
  pimpl = new vgui_parent_child_link_impl(p, c.operator->());
  pimpl->acquire();
}

vgui_parent_child_link::vgui_parent_child_link(vgui_parent_child_link const &that)
{
  pimpl = that.pimpl;

  if (pimpl)
    pimpl->acquire();
}

vgui_parent_child_link::~vgui_parent_child_link()
{
  if (pimpl)
    pimpl->release();

  pimpl = 0;
}

vgui_parent_child_link &vgui_parent_child_link::operator=(vgui_parent_child_link const &that)
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

vgui_tableau_sptr vgui_parent_child_link::parent() const
{
  return pimpl ? pimpl->parent() : 0;
}

vgui_tableau_sptr vgui_parent_child_link::child()  const
{
  return pimpl ? pimpl->child () : 0;
}

bool vgui_parent_child_link::operator==(vgui_tableau_sptr const &t) const
{
  return child() == t;
}

void vgui_parent_child_link::assign(vgui_tableau_sptr const &t)
{
  if (pimpl)
    pimpl->assign(t.operator->());
  else
  {
    vgui_macro_warning << "attempted assign() to empty parent_child_link.\n"
                       << "t = " << t << vcl_endl;
    assert(false);
  }
}

bool vgui_parent_child_link::handle(vgui_event const &e)
{
  if (!pimpl) return false;
  vgui_tableau* c = pimpl->child();
  if (!c) return false;

  return c->handle(e);
}

vgui_parent_child_link::operator bool() const
{
  return pimpl && (pimpl->child() != 0);
}

vgui_parent_child_link::operator vgui_tableau_sptr() const
{
  return pimpl ? pimpl->child() : 0;
}

vgui_tableau *vgui_parent_child_link::operator->() const
{
  return pimpl ? pimpl->child() : 0;
}

vcl_ostream & operator<<(vcl_ostream &os, vgui_parent_child_link const &s)
{
  // the reason for the flush() is to get as much stuff as
  // possible printed before an eventual segfault.
  return os << "vgui_parent_child_link("
            << vcl_flush
            << static_cast<void*>( s.parent().operator->() ) << ", "
            << vcl_flush
            << static_cast<void*>( s.child ().operator->() ) << ')'
            << vcl_flush;
}

//------------------------------------------------------------------------------

void vgui_parent_child_link::get_children_of(vgui_tableau_sptr const& tab,
                                             vcl_vector<vgui_tableau_sptr> *children)
{
  for (vcl_set<void*>::iterator i=vgui_parent_child_link_impl::all->begin();
       i!=vgui_parent_child_link_impl::all->end(); ++i)
  {
    vgui_parent_child_link_impl *ptr = static_cast<vgui_parent_child_link_impl*>(*i);
    if ( ptr->parent() == tab.operator->() )
      children->push_back( ptr->child() );
  }
}

void vgui_parent_child_link::get_parents_of (vgui_tableau_sptr const& tab,
                                             vcl_vector<vgui_tableau_sptr> *parents)
{
#if cache_parents
  vcl_vector<vgui_tableau*> const &vec
    = tab->vgui_parent_child_link_data::parents;
  for (unsigned i=0; i<vec.size(); ++i)
    parents->push_back(vec[i]);
#else
  for (vcl_set<void*>::iterator i=vgui_parent_child_link_impl::all->begin();
    i!=vgui_parent_child_link_impl::all->end(); ++i)
    {
    vgui_parent_child_link_impl *ptr
      = static_cast<vgui_parent_child_link_impl*>(*i);
    if ( ptr->child() == tab.operator->() )
      children->push_back( ptr->parent() );
  }
#endif
}

void vgui_parent_child_link::replace_child_everywhere(vgui_tableau_sptr const &old_child,
                                                      vgui_tableau_sptr const &new_child)
{
#ifdef DEBUG
  vcl_cerr << "vgui_parent_child_link::replace_child_everywhere\n"
           << "  old_child : " << old_child->pretty_name() << '\t'
           << "  new child : " << new_child->pretty_name() << '\n';
#endif

  if (old_child == new_child)
    vcl_cerr << "vgui_parent_child_link::replace_child_everywhere: old_child == new_child\n";

  for (vcl_set<void*>::iterator i=vgui_parent_child_link_impl::all->begin();
    i!=vgui_parent_child_link_impl::all->end(); ++i)
  {
    vgui_parent_child_link_impl *ptr
      = static_cast<vgui_parent_child_link_impl*>(*i);

#ifdef DEBUG
    vcl_cerr << "  parent_child_link\t"
             << "parent : " << ptr->parent()->pretty_name()
             << "\tchild : ";
    if (! ptr->child())
      vcl_cerr << "0\n";
    else
      vcl_cerr << ptr->child()->pretty_name() << '\n';
#endif

    if ( ptr->child() == old_child.operator->() ) {
      assert(ptr->parent() != new_child.operator->() );
#ifdef DEBUG
      vcl_cerr << "  replaced by: " << ptr->child() << '\n';
#endif
      ptr->parent()->notify_replaced_child(old_child, new_child);
      ptr->assign(new_child.operator->());
    }
  }
}
