// This is core/vbl/vbl_basic_relation.txx
#ifndef vbl_basic_relation_txx_
#define vbl_basic_relation_txx_
//:
// \file

#include "vbl_basic_relation.h"
#include <vbl/vbl_basic_relation_where.h>
#include <vcl_iostream.h>

// First the iterator methods.

//------------------------------------------------------------
//: Constructor.
// The iterator controls the implementation passed in, and must delete it when it gets deleted.
template <class T1, class T2, class T3, class T4, class T5>
vbl_basic_relation_iterator<T1,T2,T3,T4,T5>::vbl_basic_relation_iterator(vbl_br_iter_impl<T1,T2,T3,T4,T5>* im)
  : impl(im)
{
}

//------------------------------------------------------------
//: Constructor.
// The iterator controls the implementation passed in, and must delete it when it gets deleted.
template <class T1, class T2, class T3, class T4, class T5>
vbl_basic_relation_iterator<T1,T2,T3,T4,T5>::vbl_basic_relation_iterator()
  : impl(0)
{
}

//------------------------------------------------------------
//: Copy constructor.
template <class T1, class T2, class T3, class T4, class T5>
vbl_basic_relation_iterator<T1,T2,T3,T4,T5>::vbl_basic_relation_iterator(vbl_basic_relation_iterator<T1,T2,T3,T4,T5> const& it)
  : impl(0)
{
  if (it.impl) impl = it.impl->copy();
}

//------------------------------------------------------------
//: Destructor.
template <class T1, class T2, class T3, class T4, class T5>
vbl_basic_relation_iterator<T1,T2,T3,T4,T5>::~vbl_basic_relation_iterator()
{
  if (impl) delete impl;
}

//------------------------------------------------------------
//: Assignment operator.
template <class T1, class T2, class T3, class T4, class T5>
vbl_basic_relation_iterator<T1,T2,T3,T4,T5>&
    vbl_basic_relation_iterator<T1,T2,T3,T4,T5>::operator=
    (vbl_basic_relation_iterator<T1,T2,T3,T4,T5> const& it)
{
  if (&it == this) return *this;
  if (impl) delete impl;
  if (it.impl) impl = it.impl->copy();
  return *this;
}

//----------------------------------------------------------------------
//: Constructor.
template <class T1, class T2, class T3, class T4, class T5>
vbl_basic_relation<T1,T2,T3,T4,T5>::vbl_basic_relation(const vcl_string& name)
  : impl(0), where(0)
{
  // Use the implementation static factory to get the existing
  // relation or build a new implementation.
  impl = implementation::GetFactory()->Generate(name);
  impl->ref();
  where = new where_clause;
}

//----------------------------------------------------------------------
//: Copy constructor.
// Both copies refer to the same relation since the names are the same.
// However the where clauses may be changed independently.
template <class T1, class T2, class T3, class T4, class T5>
vbl_basic_relation<T1,T2,T3,T4,T5>::vbl_basic_relation(vbl_basic_relation<T1,T2,T3,T4,T5> const& erel)
  : impl(erel.impl), where(0)
{
  if (!impl)
  {
    vcl_cerr << "Copy constructor called with invalid easy relation\n";
    vcl_abort();
  }
  impl->ref();

  where = new where_clause(*erel.where);
}

//----------------------------------------------------------------------
//: Destructor gets rid of this reference to the relation implementation.
// The last reference to the implementation will
// cause it to be deleted if it is clear at the time.
template <class T1, class T2, class T3, class T4, class T5>
vbl_basic_relation<T1,T2,T3,T4,T5>::~vbl_basic_relation()
{
  // This will never cause the implementation to go away because the
  // factory also protects it.
  impl->unref();
  delete where;

  // The factory protects all implementations.  This call instructs
  // the factory to check whether the given implementation is empty,
  // and if it is also only referenced by the factory (ie. protected
  // only once) to unprotect it.
  implementation::GetFactory()->CheckEmpty(impl);
}

//------------------------------------------------------------
//: Return an iterator for the start of this relation.
template <class T1, class T2, class T3, class T4, class T5>
vbl_basic_relation_iterator<T1,T2,T3,T4,T5> vbl_basic_relation<T1,T2,T3,T4,T5>::begin()
{
  return vbl_basic_relation_iterator<T1,T2,T3,T4,T5>(impl->begin(*where));
}

//------------------------------------------------------------
//: Return an iterator which equals any iterator which has reached the end of a relation.
template <class T1, class T2, class T3, class T4, class T5>
vbl_basic_relation_iterator<T1,T2,T3,T4,T5> vbl_basic_relation<T1,T2,T3,T4,T5>::end()
{
  return vbl_basic_relation_iterator<T1,T2,T3,T4,T5>(impl->end(*where));
}

//------------------------------------------------------------
//: Clear a relation.
// Clears all the tuples selected by the current where clause.
// Returns true if one or more tuples removed.
template <class T1, class T2, class T3, class T4, class T5>
bool vbl_basic_relation<T1,T2,T3,T4,T5>::clear()
{
  return impl->Remove(*where);
}

//------------------------------------------------------------
//: Remove all tuples with the given first attribute.
//  Returns true if one or more tuples removed.
template <class T1, class T2, class T3, class T4, class T5>
bool vbl_basic_relation<T1,T2,T3,T4,T5>::remove_first(T1 t1)
{
  return where_first(t1).clear();
}

//------------------------------------------------------------
//: Remove all tuples with the given second attribute.
//  Returns true if one or more tuples removed.
template <class T1, class T2, class T3, class T4, class T5>
bool vbl_basic_relation<T1,T2,T3,T4,T5>::remove_second(T2 t2)
{
  return where_second(t2).clear();
}

//------------------------------------------------------------
//: Remove all tuples with the given third attribute.
//  Returns true if one or more tuples removed.
template <class T1, class T2, class T3, class T4, class T5>
bool vbl_basic_relation<T1,T2,T3,T4,T5>::remove_third(T3 t3)
{
  return where_third(t3).clear();
}

//------------------------------------------------------------
//: Remove all tuples with the given fourth attribute.
//  Returns true if one or more tuples removed.
template <class T1, class T2, class T3, class T4, class T5>
bool vbl_basic_relation<T1,T2,T3,T4,T5>::remove_fourth(T4 t4)
{
  return where_fourth(t4).clear();
}

//------------------------------------------------------------
//: Remove all tuples with the given fifth attribute.
// Returns true if one or more tuples removed.
template <class T1, class T2, class T3, class T4, class T5>
bool vbl_basic_relation<T1,T2,T3,T4,T5>::remove_fifth(T5 t5)
{
  return where_fifth(t5).clear();
}

//------------------------------------------------------------
//: Returns an vbl_basic_relation with a where clause selecting those tuples with the given first attribute.
template <class T1, class T2, class T3, class T4, class T5>
vbl_basic_relation<T1,T2,T3,T4,T5> vbl_basic_relation<T1,T2,T3,T4,T5>::where_first(T1 t1)
{
  vbl_basic_relation<T1,T2,T3,T4,T5> new_rel(*this);
  new_rel.where->SetFirst(t1);
  return new_rel;
}

//------------------------------------------------------------
//: Returns an vbl_basic_relation with a where clause selecting those tuples with the given second attribute.
template <class T1, class T2, class T3, class T4, class T5>
vbl_basic_relation<T1,T2,T3,T4,T5> vbl_basic_relation<T1,T2,T3,T4,T5>::where_second(T2 t2)
{
  vbl_basic_relation<T1,T2,T3,T4,T5> new_rel(*this);
  new_rel.where->SetSecond(t2);
  return new_rel;
}

//------------------------------------------------------------
//: Returns an vbl_basic_relation with a where clause selecting those tuples with the given third attribute.
template <class T1, class T2, class T3, class T4, class T5>
vbl_basic_relation<T1,T2,T3,T4,T5> vbl_basic_relation<T1,T2,T3,T4,T5>::where_third(T3 t3)
{
  vbl_basic_relation<T1,T2,T3,T4,T5> new_rel(*this);
  new_rel.where->SetThird(t3);
  return new_rel;
}

//------------------------------------------------------------
//: Returns an vbl_basic_relation with a where clause selecting those tuples with the given fourth attribute.
template <class T1, class T2, class T3, class T4, class T5>
vbl_basic_relation<T1,T2,T3,T4,T5> vbl_basic_relation<T1,T2,T3,T4,T5>::where_fourth(T4 t4)
{
  vbl_basic_relation<T1,T2,T3,T4,T5> new_rel(*this);
  new_rel.where->SetFourth(t4);
  return new_rel;
}

//------------------------------------------------------------
//: Returns an vbl_basic_relation with a where clause selecting those tuples with the given fifth attribute.
template <class T1, class T2, class T3, class T4, class T5>
vbl_basic_relation<T1,T2,T3,T4,T5> vbl_basic_relation<T1,T2,T3,T4,T5>::where_fifth(T5 t5)
{
  vbl_basic_relation<T1,T2,T3,T4,T5> new_rel(*this);
  new_rel.where->SetFifth(t5);
  return new_rel;
}

//------------------------------------------------------------
//: Returns an vbl_basic_relation with a where clause given by the user.
template <class T1, class T2, class T3, class T4, class T5>
vbl_basic_relation<T1,T2,T3,T4,T5>
vbl_basic_relation<T1,T2,T3,T4,T5>::where_is(const vbl_basic_relation_where<T1,T2,T3,T4,T5>& where)
{
  vbl_basic_relation<T1,T2,T3,T4,T5> new_rel(*this);
  (*new_rel.where) = where;
  return new_rel;
}

//------------------------------------------------------------
//: Dump the relation onto the given stream.
template <class T1, class T2, class T3, class T4, class T5>
void vbl_basic_relation<T1,T2,T3,T4,T5>::dump_relation(vcl_ostream& str)
{
  str << "Relation " << get_name() << ":\n";
  where->Dump(str);
  for (iterator i = begin(); i != end(); i++)
    str << '\t' << *i << vcl_endl;
}

//------------------------------------------------------------
//: Return the address of a specific static variable, unique to this class.
template <class T1, class T2, class T3, class T4, class T5>
void* vbl_basic_relation<T1,T2,T3,T4,T5>::get_type()
{
  return vbl_basic_relation<T1,T2,T3,T4,T5>::get_static_type();
}

//------------------------------------------------------------
//: Return the correct pointer to "this" as a specific type.
template <class T1, class T2, class T3, class T4, class T5>
void* vbl_basic_relation<T1,T2,T3,T4,T5>::get_this()
{
  return (void*)this;
}

//------------------------------------------------------------
//: Static partner of get_type().
template <class T1, class T2, class T3, class T4, class T5>
void* vbl_basic_relation<T1,T2,T3,T4,T5>::get_static_type()
{
  static int type_static = 0;
  return (void*)&type_static;
}

//------------------------------------------------------------
//: Method to actually do a downcast.  Return NULL if r is not this type.
//  This is a static.
template <class T1, class T2, class T3, class T4, class T5>
vbl_basic_relation<T1,T2,T3,T4,T5>*
vbl_basic_relation<T1,T2,T3,T4,T5>::cast(vbl_basic_relation_type* r)
{
  if (!r) return NULL;
  // Cast is legal if types agree.
  if (vbl_basic_relation<T1,T2,T3,T4,T5>::get_static_type() == r->GetType())
    return (vbl_basic_relation<T1,T2,T3,T4,T5> *)(r->This());
  else
    return NULL;
}

#endif // vbl_basic_relation_txx_
