// This is core/vbl/vbl_br_default.txx
#ifndef vbl_br_default_txx_
#define vbl_br_default_txx_

//:
// \file

#include "vbl_br_default.h"
#include <vbl/vbl_basic_optional.h>
#include <vcl_cassert.h>

//--------------------------------------------------------------------------------
// Methods for the regular iterator.

//------------------------------------------------------------
//: Constructor.  This iterator is at end.
template <class T1, class T2, class T3, class T4, class T5>
vbl_br_default_iter<T1,T2,T3,T4,T5>::vbl_br_default_iter()
  : vbl_br_iter_impl<T1,T2,T3,T4,T5>(), where(0), tlist(0)
{
}

//------------------------------------------------------------
//: Constructor.
template <class T1, class T2, class T3, class T4, class T5>
vbl_br_default_iter<T1,T2,T3,T4,T5>::vbl_br_default_iter(vbl_br_default<T1,T2,T3,T4,T5>* impl,
                                                         vbl_basic_relation_where<T1,T2,T3,T4,T5> const* w)
  : vbl_br_iter_impl<T1,T2,T3,T4,T5>(), where(0), tlist(0)
{
  assert((impl != 0) && (w != 0));

  // Copy the where clause.
  where = new where_clause(*w);

  // Get the primary key for the where clause.
  int key = where->PrimaryKey();

  // This iterator is only to be used when the where clause has at
  // least one non-wild attribute.
  assert(key != 0);

  // Set up the appropriate list.
  if (key == 1)
  {
    if (impl->i1.cool_find(where->GetFirst()))
      tlist = &(impl->i1.value());
  }
  else if (key == 2)
  {
    if (impl->i2.cool_find(where->GetSecond()))
      tlist = &(impl->i2.value());
  }
  else if (!vbl_basic_optional_traits<T3>::IsOptional && (key == 3))
  {
    if (impl->i3.cool_find(where->GetThird()))
      tlist = &(impl->i3.value());
  }
  else if (!vbl_basic_optional_traits<T4>::IsOptional && (key == 4))
  {
    if (impl->i4.cool_find(where->GetFourth()))
      tlist = &(impl->i4.value());
  }
  else if (!vbl_basic_optional_traits<T5>::IsOptional && (key == 5))
  {
    if (impl->i5.cool_find(where->GetFifth()))
      tlist = &(impl->i5.value());
  }

  if (tlist) iter = tlist->begin();

  // Step until match found.
  FindMatch();
}

//------------------------------------------------------------
//: Copy constructor.
template <class T1, class T2, class T3, class T4, class T5>
vbl_br_default_iter<T1,T2,T3,T4,T5>::vbl_br_default_iter(vbl_br_default_iter<T1,T2,T3,T4,T5> const& it)
  : vbl_br_iter_impl<T1,T2,T3,T4,T5>(it),
    where(0), tlist(it.tlist), iter(it.iter)
{
  // Copy the where clause.
  where = new where_clause(*(it.where));
}

//------------------------------------------------------------
//: Destructor.
template <class T1, class T2, class T3, class T4, class T5>
vbl_br_default_iter<T1,T2,T3,T4,T5>::~vbl_br_default_iter()
{
  if (where) delete where;
}

//------------------------------------------------------------
//: Make a copy of this instance.
template <class T1, class T2, class T3, class T4, class T5>
vbl_br_iter_impl<T1,T2,T3,T4,T5>* vbl_br_default_iter<T1,T2,T3,T4,T5>::copy() const
{
  return new vbl_br_default_iter(*this);
}

//------------------------------------------------------------
//: Dereference to return the current tuple.
template <class T1, class T2, class T3, class T4, class T5>
vbl_basic_tuple<T1,T2,T3,T4,T5>*
vbl_br_default_iter<T1,T2,T3,T4,T5>::deref() const
{
  if (AtEnd()) return 0;
  else return (tuple*)*iter;
}

//------------------------------------------------------------
//: Increment to point to the next selected tuple.
template <class T1, class T2, class T3, class T4, class T5>
void vbl_br_default_iter<T1,T2,T3,T4,T5>::incr()
{
  if (AtEnd()) return;
  ++iter;
  FindMatch();
}

//------------------------------------------------------------
//: Compare two iterators.
//  Since these are both from the same relation, and tuples are unique within
// the relation, we can just compare the addresses of tuples.
template <class T1, class T2, class T3, class T4, class T5>
bool vbl_br_default_iter<T1,T2,T3,T4,T5>::compare(vbl_br_iter_impl<T1,T2,T3,T4,T5> const& i) const
{
  return deref() == i.deref();
}

//------------------------------------------------------------
//: Step iterator through relation until match is valid.
//  Starts by testing the current position, so this does not necessarily
// perform an increment.
template <class T1, class T2, class T3, class T4, class T5>
void vbl_br_default_iter<T1,T2,T3,T4,T5>::FindMatch()
{
  if (!tlist) return;

  // Iterate over all possible tuples until a match is found.
  while ((iter != tlist->end()) && (!where->match((tuple*)*iter)))
    ++iter;

  // Invar: (iter == tlist->end()) || (*iter) matches where)
}

//------------------------------------------------------------
//: Return true if *this == impl.end().
template <class T1, class T2, class T3, class T4, class T5>
bool vbl_br_default_iter<T1,T2,T3,T4,T5>::AtEnd() const
{
  return (!tlist) || (iter == tlist->end());
}

//--------------------------------------------------------------------------------
// Methods for the wild iterator.

//------------------------------------------------------------
//: Constructor.
template <class T1, class T2, class T3, class T4, class T5>
vbl_br_default_wild_iter<T1,T2,T3,T4,T5>::vbl_br_default_wild_iter(vbl_br_default<T1,T2,T3,T4,T5>* init_impl,
                                                                   vbl_basic_relation_where<T1,T2,T3,T4,T5> const* w)
  : vbl_br_iter_impl<T1,T2,T3,T4,T5>()
{
  if (w)
  {
    // This iterator is only to be used when the where clause has all
    // wild attributes.
    assert(w->PrimaryKey() == 0);
  }

  impl = init_impl;
  impl->ref();

  // Set first iterator to start of first index.
  impl->i1.reset();
  // If first index is not empty...
  if (impl->i1.next())
  {
    // ...set second iterators to second list bounds.
    i2 = (impl->i1.value()).begin();
    i2_end = (impl->i1.value()).end();
  }

  // Save position in first index.
  i1 = impl->i1.current_position();
}

//------------------------------------------------------------
//: Copy constructor.
template <class T1, class T2, class T3, class T4, class T5>
vbl_br_default_wild_iter<T1,T2,T3,T4,T5>::vbl_br_default_wild_iter(vbl_br_default_wild_iter<T1,T2,T3,T4,T5> const& it)
  : vbl_br_iter_impl<T1,T2,T3,T4,T5>(it),
    i1(it.i1), impl(it.impl), i2(it.i2), i2_end(it.i2_end)
{
  impl->ref();
}

//------------------------------------------------------------
//: Destructor.
template <class T1, class T2, class T3, class T4, class T5>
vbl_br_default_wild_iter<T1,T2,T3,T4,T5>::~vbl_br_default_wild_iter()
{
  impl->unref();
}

//------------------------------------------------------------
//: Make a copy of this instance.
template <class T1, class T2, class T3, class T4, class T5>
vbl_br_iter_impl<T1,T2,T3,T4,T5>* vbl_br_default_wild_iter<T1,T2,T3,T4,T5>::copy() const
{
  return new vbl_br_default_wild_iter(*this);
}

//------------------------------------------------------------
//: Dereference to return the current tuple.
//  Returns a pointer, which is null if dereference is invalid for current position.
template <class T1, class T2, class T3, class T4, class T5>
vbl_basic_tuple<T1,T2,T3,T4,T5>* vbl_br_default_wild_iter<T1,T2,T3,T4,T5>::deref() const
{
  // Note i2 can always be dereferenced if i1 is valid.
  if (i1 == impl->i1.end()) return 0;
  else return (tuple*)*i2;
}

//------------------------------------------------------------
//: Increment
template <class T1, class T2, class T3, class T4, class T5>
void vbl_br_default_wild_iter<T1,T2,T3,T4,T5>::incr()
{
  // Check that we are not already at end.
  if (i1 != impl->i1.end())
  {
    // Increment list iterator.
    ++i2;
    // Have we reached the end of the current list?
    if (i2 == i2_end)
    {
      // Yes, so move to next list.
      impl->i1.current_position() = i1;
      if (impl->i1.next())
      {
        // ...set second iterators to second list bounds.
        i2 = (impl->i1.value()).begin();
        i2_end = (impl->i1.value()).end();
      }
      // Save position in first index.
      i1 = impl->i1.current_position();
    }
  }
}

//------------------------------------------------------------
//: Compare two iterators.
template <class T1, class T2, class T3, class T4, class T5>
bool vbl_br_default_wild_iter<T1,T2,T3,T4,T5>::compare(vbl_br_iter_impl<T1,T2,T3,T4,T5> const& i) const
{
  return deref() == i.deref();
}


//--------------------------------------------------------------------------------
// Methods for the relation implementation.

//------------------------------------------------------------
//: Constructor.  Calls the parent constructor.
template <class T1, class T2, class T3, class T4, class T5>
vbl_br_default<T1,T2,T3,T4,T5>::vbl_br_default(vcl_string const& n)
  : vbl_br_impl<T1,T2,T3,T4,T5>(n),
    u1(false), u2(false), u3(false), u4(false), u5(false)
{
}

//------------------------------------------------------------
//: Copy constructor.
template <class T1, class T2, class T3, class T4, class T5>
vbl_br_default<T1,T2,T3,T4,T5>::vbl_br_default(vbl_br_default<T1,T2,T3,T4,T5> const& i)
  : vbl_br_impl<T1,T2,T3,T4,T5>(i),
    u1(i.u1), u2(i.u2), u3(i.u3), u4(i.u4), u5(i.u5)
{
  // Copy the relation.  All tuples will appear in each map, so it is
  // sufficient to insert all the tuples found in the first map.  Note
  // here that we use the non-virtual insert method, since we are
  // still in the constructor.
  //

  // Have to cast away const here.
  first_map* map = (first_map*)&(i.i1);
  for (map->reset(); map->next();)
  {
    const tuple_list& tlist = map->value();

    // Search for tuple.
    for (tuple_list::const_iterator j = tlist.begin(); j != tlist.end(); j++)
      DoInsert(*(tuple*)(*j));
  }
}

//------------------------------------------------------------
//: Destructor.
template <class T1, class T2, class T3, class T4, class T5>
vbl_br_default<T1,T2,T3,T4,T5>::~vbl_br_default()
{
}

//------------------------------------------------------------
//: Add a tuple.  Return false if tuple already in relation.
template <class T1, class T2, class T3, class T4, class T5>
bool vbl_br_default<T1,T2,T3,T4,T5>::Insert(vbl_basic_tuple<T1,T2,T3,T4,T5> const& t)
{
  // Call the non-virtual method.
  return DoInsert(t);
}

//------------------------------------------------------------
//: Remove a tuple.  Return true if tuple was in relation.
template <class T1, class T2, class T3, class T4, class T5>
bool vbl_br_default<T1,T2,T3,T4,T5>::Remove(vbl_basic_tuple<T1,T2,T3,T4,T5> const& t)
{
  vbl_basic_tuple<T1,T2,T3,T4,T5>* found = 0;
  {
    // Find tuple key in first index.
    first_map::iterator fp = i1.find(t.GetFirst());
    if (fp == i1.end())
      return false;
    tuple_list& tlist = (*fp).second;

    // Search for tuple.
    for (tuple_list::iterator i = tlist.begin(); i != tlist.end(); i++)
      // In this case test for tuple equality.
      if (*(tuple*)(*i) == t)
      {
        // Found tuple, so erase it.
        found = (tuple*)*i;
        tlist.erase(i);
        break;
      }

    // Was the exact tuple found.
    if (!found) return false;

    // Remove the key if the list is empty.
    if (tlist.empty()) i1.erase(fp);
  }

  // Now we have the tuple address in relation, so remove this from other indices.
  {
    // Find tuple in second index.
    second_map::iterator fp2 = i2.find(t.GetSecond());
    tuple_list& tlist = (*fp2).second;
    for (tuple_list::iterator i = tlist.begin(); i != tlist.end(); i++)
      if (*i == found) { tlist.erase(i); break; }
    // Remove the key if the list is empty.
    if (tlist.empty()) i2.erase(fp2);
  }

  if (!vbl_basic_optional_traits<T3>::IsOptional)
  {
    // Find tuple in third index.
    third_map::iterator fp3 = i3.find(t.GetThird());
    tuple_list& tlist = (*fp3).second;
    for (tuple_list::iterator i = tlist.begin(); i != tlist.end(); i++)
      if (*i == found) { tlist.erase(i); break; }
    // Remove the key if the list is empty.
    if (tlist.empty()) i3.erase(fp3);
  }

  if (!vbl_basic_optional_traits<T4>::IsOptional)
  {
    // Find tuple in fourth index.
    fourth_map::iterator fp4 = i4.find(t.GetFourth());
    tuple_list& tlist = (*fp4).second;
    for (tuple_list::iterator i = tlist.begin(); i != tlist.end(); i++)
      if (*i == found) { tlist.erase(i); break; }
    // Remove the key if the list is empty.
    if (tlist.empty()) i4.erase(fp4);
  }

  if (!vbl_basic_optional_traits<T5>::IsOptional)
  {
    // Find tuple in fifth index.
    fourth_map::iterator fp5 = i5.find(t.GetFifth());
    tuple_list& tlist = (*fp5).second;
    for (tuple_list::iterator i = tlist.begin(); i != tlist.end(); i++)
      if (*i == found) { tlist.erase(i); break; }
    // Remove the key if the list is empty.
    if (tlist.empty()) i5.erase(fp5);
  }

  // Delete the tuple found.
  delete found;

  return true;
}

//------------------------------------------------------------
//: Remove the tuples matching a where clause.
//  This uses wild cards to search.  Return true if at least one tuple removed.
template <class T1, class T2, class T3, class T4, class T5>
bool vbl_br_default<T1,T2,T3,T4,T5>::Remove(vbl_basic_relation_where<T1,T2,T3,T4,T5> const& w)
{
  bool removed_tuples = false;

  bool done = false;
  while (!done)
  {
    iterator* iter = begin(w);
    if (!iter->deref()) done = true;
    else
    {
      vbl_basic_tuple<T1,T2,T3,T4,T5>* t = (tuple*)iter->deref();

      // Remove the matching tuple.
      removed_tuples = true;

      // *t also gets deleted in here, but that's fine.
      Remove(*t);
    }
    delete iter;
  }

  // Return true if we removed at least one tuple.
  return removed_tuples;
}

//------------------------------------------------------------
//: Get an iterator for a given where clause.
template <class T1, class T2, class T3, class T4, class T5>
vbl_br_iter_impl<T1,T2,T3,T4,T5>* vbl_br_default<T1,T2,T3,T4,T5>::begin(vbl_basic_relation_where<T1,T2,T3,T4,T5> const& w)
{
  int key = w.PrimaryKey();
  if (key == 0)
    return new vbl_br_default_wild_iter<T1,T2,T3,T4,T5>(this,&w);
  else
    return new vbl_br_default_iter<T1,T2,T3,T4,T5>(this,&w);
}

//------------------------------------------------------------
//: Get an iterator for the end of a where clause.
template <class T1, class T2, class T3, class T4, class T5>
vbl_br_iter_impl<T1,T2,T3,T4,T5>* vbl_br_default<T1,T2,T3,T4,T5>::end(vbl_basic_relation_where<T1,T2,T3,T4,T5> const&)
{
  return new vbl_br_default_iter<T1,T2,T3,T4,T5>;
}

//------------------------------------------------------------
//: Get the number of tuples in this relation which match the where clause.
template <class T1, class T2, class T3, class T4, class T5>
int vbl_br_default<T1,T2,T3,T4,T5>::size(vbl_basic_relation_where<T1,T2,T3,T4,T5> const& w)
{
  // Make an iterator.
  iterator* iter = begin(w);
  iterator* iter_end = end(w);

  int count = 0;
  while (!iter->compare(*iter_end))
  {
    count++;
    iter->incr();
  }

  delete iter;
  delete iter_end;
  return count;
}

//------------------------------------------------------------
//: Return true if no tuples match the where clause.
template <class T1, class T2, class T3, class T4, class T5>
bool vbl_br_default<T1,T2,T3,T4,T5>::empty(vbl_basic_relation_where<T1,T2,T3,T4,T5> const& w)
{
  // Make an iterator.
  iterator* iter = begin(w);
  iterator* iter_end = end(w);

  // Relation is empty under this where clause if the iterator == end.
  bool is_empty = (iter->compare(*iter_end));

  delete iter;
  delete iter_end;
  return is_empty;
}

//------------------------------------------------------------
//: Return the address of this, used for downcasts.
template <class T1, class T2, class T3, class T4, class T5>
void* vbl_br_default<T1,T2,T3,T4,T5>::This()
{
  return this;
}

//------------------------------------------------------------
//: Add a tuple.
//  This method is used in the constructor hence it is non-virtual.
// Return false if tuple is already present.
template <class T1, class T2, class T3, class T4, class T5>
bool vbl_br_default<T1,T2,T3,T4,T5>::DoInsert(vbl_basic_tuple<T1,T2,T3,T4,T5> const& t)
{
  tuple* tr;

  // Do checks on each unique attribute.
  if (u1 && i1.cool_find(t.GetFirst())) return false;
  if (u2 && i2.cool_find(t.GetSecond())) return false;
  if ((!vbl_basic_optional_traits<T3>::IsOptional) &&
      u3 && i3.cool_find(t.GetThird())) return false;
  if ((!vbl_basic_optional_traits<T4>::IsOptional) &&
      u4 && i4.cool_find(t.GetFourth())) return false;
  if ((!vbl_basic_optional_traits<T5>::IsOptional) &&
      u5 && i5.cool_find(t.GetFifth())) return false;

  // Insert into the first index, searching to make sure this tuple is
  // not already there.
  {
    if (!i1.cool_find(t.GetFirst()))
    {
      tuple_list new_tlist;
      i1.insert(t.GetFirst(),new_tlist);
    }
    tuple_list& tlist = i1.value();
    bool found = false;
    for (tuple_list::iterator i = tlist.begin(); !(found || (i == tlist.end())); i++)
      if (*(tuple*)(*i) == t) found = true;
    if (found) return false;
    tr = new vbl_basic_tuple<T1,T2,T3,T4,T5>(t);
    tlist.push_front(tr);
  }

  // Now we can safely insert into all the other indices.
  {
    if (!i2.cool_find(t.GetSecond()))
    {
      tuple_list new_tlist;
      i2.insert(t.GetSecond(),new_tlist);
    }
    tuple_list& tlist = i2.value();
    tlist.push_front(tr);
  }

  if (!vbl_basic_optional_traits<T3>::IsOptional)
  {
    if (!i3.cool_find(t.GetThird()))
    {
      tuple_list new_tlist;
      i3.insert(t.GetThird(),new_tlist);
    }
    tuple_list& tlist = i3.value();
    tlist.push_front(tr);
  }

  if (!vbl_basic_optional_traits<T4>::IsOptional)
  {
    if (!i4.cool_find(t.GetFourth()))
    {
      tuple_list new_tlist;
      i4.insert(t.GetFourth(),new_tlist);
    }
    tuple_list& tlist = i4.value();
    tlist.push_front(tr);
  }

  if (!vbl_basic_optional_traits<T5>::IsOptional)
  {
    if (!i5.cool_find(t.GetFifth()))
    {
      tuple_list new_tlist;
      i5.insert(t.GetFifth(),new_tlist);
    }
    tuple_list& tlist = i5.value();
    tlist.push_front(tr);
  }

  return true;
}


//--------------------------------------------------------------------------------
// Methods for the factory for this implementation.

template <class T1, class T2, class T3, class T4, class T5>
vbl_br_default_factory<T1,T2,T3,T4,T5>::~vbl_br_default_factory()
{
  for (existing_map::iterator p = existing.begin(); p != existing.end(); ++p)
  {
    implementation* impl = (implementation*)(*p).second;
    CheckEmpty(impl);
  }
}

//------------------------------------------------------------
//: Either return existing implementation of that name, or generate a new one.
template <class T1, class T2, class T3, class T4, class T5>
vbl_br_impl<T1,T2,T3,T4,T5>* vbl_br_default_factory<T1,T2,T3,T4,T5>::Generate(vcl_string name)
{
  existing_map::iterator fp = existing.find(name);
  if (fp != existing.end())
    return (implementation*)(*fp).second;

  // Construct new default implementation.
  vbl_br_default<T1,T2,T3,T4,T5>* i = new vbl_br_default<T1,T2,T3,T4,T5>(name);
  i->ref();
  existing.insert(name,i);
  return i;
}

//------------------------------------------------------------
//: Check whether the implementation is empty, and only protected here.
//  If so then remove and delete it.
template <class T1, class T2, class T3, class T4, class T5>
void vbl_br_default_factory<T1,T2,T3,T4,T5>::CheckEmpty(vbl_br_impl<T1,T2,T3,T4,T5>* impl)
{
  // Check that impl is in existing map.
  existing_map::iterator fp = existing.find(impl->GetName());
  if (fp != existing.end())
    return;

  // Are we the only ones protecting this implementation?
  if (impl->get_references() == 1)
  {
    vbl_basic_relation_where<T1,T2,T3,T4,T5> w;
    if (impl->empty(w))
    {
      // Yes, so erase it and unprotect it.
      existing.erase(fp);
      impl->unref();
    }
  }
}

#endif // vbl_br_default_txx_
