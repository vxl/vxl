//-*- c++ -*-
#ifndef vbl_br_default_h
#define vbl_br_default_h

//-----------------------------------------------------------------------------
//
// .LIBRARY vbl
// .HEADER Basics Package
// .INCLUDE vbl/vbl_br_default.h
// .FILE vbl/vbl_br_default.cxx
//
// .SECTION Author:
//             Rupert Curwen, January 14th, 1998
//             GE Corporate Research and Development
//
// .SECTION Modifications
//     None Yet
//
//-----------------------------------------------------------------------------

#ifdef _MSC_VER
// disable annoying MSC warning about symbols greater than 255
// characters (templates produce lots of these).
#pragma warning(disable:4786)
#endif // _MSC_VER

#include <vcl/vcl_list.h>
#include <vcl/vcl_string.h>

#include <vbl/vbl_br_impl.h>
#include <vbl/vbl_basic_relation.h>
#include <vbl/vbl_basic_relation_where.h>
#include <vbl/vbl_basic_optional.h>
#include <vbl/vbl_hash_map.h>

// Forward declare the implementation.
template <class T1, class T2, 
  VCL_DFL_TYPE_PARAM_STLDECL(T3,vbl_basic_optional), 
  VCL_DFL_TYPE_PARAM_STLDECL (T4, vbl_basic_optional), 
  VCL_DFL_TYPE_PARAM_STLDECL(T5,vbl_basic_optional)>
class vbl_br_default;

//------------------------------------------------------------
// -- This iterator is used when the where clause has at least one
// non-wild element.
template <class T1, class T2, 
  VCL_DFL_TYPE_PARAM_STLDECL(T3,vbl_basic_optional), 
  VCL_DFL_TYPE_PARAM_STLDECL (T4, vbl_basic_optional), 
  VCL_DFL_TYPE_PARAM_STLDECL(T5,vbl_basic_optional)>
class vbl_br_default_iter : public vbl_br_iter_impl<T1,T2,T3,T4,T5>
{
public:
  typedef vbl_br_iter_impl<T1,T2,T3,T4,T5> base;
  typedef vbl_basic_relation_where<T1,T2,T3,T4,T5> where_clause;
  typedef vbl_br_default<T1,T2,T3,T4,T5> implementation;
  typedef vcl_list<void*> tuple_list;

  typedef vbl_basic_tuple<T1,T2,T3,T4,T5> tuple;

public:
  vbl_br_default_iter();
  vbl_br_default_iter(implementation* i, const vbl_basic_relation_where<T1,T2,T3,T4,T5>* w);
  vbl_br_default_iter(const vbl_br_default_iter<T1,T2,T3,T4,T5>& it);
  virtual ~vbl_br_default_iter();

  // Make a copy of this instance.
  virtual vbl_br_iter_impl<T1,T2,T3,T4,T5>* copy() const;

  // Dereference.
  virtual vbl_basic_tuple<T1,T2,T3,T4,T5>* deref() const;

  // Increment.
  virtual void incr();

  // Compare.
  virtual bool compare(const vbl_br_iter_impl<T1,T2,T3,T4,T5>& i) const;

protected:
  void FindMatch();
  bool AtEnd() const;

protected:
  where_clause* where;
  tuple_list* tlist;
  tuple_list::iterator iter;
};

//------------------------------------------------------------
// -- This iterator is used when the where clause has all wild elements.
template <class T1, class T2, 
  VCL_DFL_TYPE_PARAM_STLDECL(T3,vbl_basic_optional), 
  VCL_DFL_TYPE_PARAM_STLDECL (T4, vbl_basic_optional), 
  VCL_DFL_TYPE_PARAM_STLDECL(T5,vbl_basic_optional)>
class vbl_br_default_wild_iter : public vbl_br_iter_impl<T1,T2,T3,T4,T5>
{
public:
  typedef vbl_basic_relation_where<T1,T2,T3,T4,T5> where_clause;
  typedef vbl_br_default<T1,T2,T3,T4,T5> implementation;
  typedef vbl_basic_tuple<T1,T2,T3,T4,T5> tuple;
  typedef vcl_list<void*> tuple_list;
  typedef vbl_hash_map<T1, tuple_list> first_map;

public:
  vbl_br_default_wild_iter(implementation* i, const vbl_basic_relation_where<T1,T2,T3,T4,T5>* w);
  vbl_br_default_wild_iter(const vbl_br_default_wild_iter<T1,T2,T3,T4,T5>& it);
  virtual ~vbl_br_default_wild_iter();

  // Make a copy of this instance.
  virtual vbl_br_iter_impl<T1,T2,T3,T4,T5>* copy() const;

  // Dereference.
  virtual vbl_basic_tuple<T1,T2,T3,T4,T5>* deref() const;

  // Increment.
  virtual void incr();

  // Compare.
  virtual bool compare(const vbl_br_iter_impl<T1,T2,T3,T4,T5>& i) const;

protected:
  first_map::iterator i1;
  implementation* impl;
  tuple_list::iterator i2; 
  tuple_list::iterator i2_end; 
};

//: Provides the default implementation for vbl_basic_relations
// It is a subclass of vbl_br_impl, and is the one
// which is used unless the factory has been set otherwise.  The
// relation is implemented using one hash table for each attribute
// as indices, with values being lists of tuple references.
//
template 
<class T1, class T2, class T3, class T4, class T5> // defaults for T3 T4 T5 already declared on line 76 -- PLEASE don't put them back here!! - PVr
class vbl_br_default : public vbl_br_impl<T1,T2,T3,T4,T5>
{

  friend class vbl_br_default_iter<T1,T2,T3,T4,T5>;
  friend class vbl_br_default_wild_iter<T1,T2,T3,T4,T5>;

public:
  typedef vcl_list<void*> tuple_list;
  typedef vbl_basic_relation_where<T1,T2,T3,T4,T5> where_clause;
  typedef vbl_br_iter_impl<T1,T2,T3,T4,T5> iterator;
  typedef vbl_basic_tuple<T1,T2,T3,T4,T5> tuple;

  typedef vbl_hash_map<T1, tuple_list> first_map;
  typedef vbl_hash_map<T2, tuple_list> second_map;
  typedef vbl_hash_map<T3, tuple_list> third_map;
  typedef vbl_hash_map<T4, tuple_list> fourth_map;
  typedef vbl_hash_map<T5, tuple_list> fifth_map;

public:
  // Constructor.
  vbl_br_default(const vcl_string& n);

  // Copy constructor.
  vbl_br_default(const vbl_br_default<T1,T2,T3,T4,T5>& i);

  // Destructor.
  virtual ~vbl_br_default();

  // Add a tuple.
  virtual bool Insert(const vbl_basic_tuple<T1,T2,T3,T4,T5>& t);

  // Remove a tuple.
  virtual bool Remove(const vbl_basic_tuple<T1,T2,T3,T4,T5>& t);

  // Remove tuples matching where clause.
  bool Remove(const vbl_basic_relation_where<T1,T2,T3,T4,T5>& w);

  // Make attributes unique.
  virtual bool FirstUnique() { return u1; }
  virtual bool SecondUnique() { return u2; }
  virtual bool ThirdUnique() { return u3; }
  virtual bool FourthUnique() { return u4; }
  virtual bool FifthUnique() { return u5; }
  virtual void SetFirstUnique(bool b) { u1 = b; }
  virtual void SetSecondUnique(bool b) { u2 = b; }
  virtual void SetThirdUnique(bool b) { u3 = b; }
  virtual void SetFourthUnique(bool b) { u4 = b; }
  virtual void SetFifthUnique(bool b) { u5 = b; }

  // Get iterators.
  virtual vbl_br_iter_impl<T1,T2,T3,T4,T5>* begin(const vbl_basic_relation_where<T1,T2,T3,T4,T5>& w);
  virtual vbl_br_iter_impl<T1,T2,T3,T4,T5>* end(const vbl_basic_relation_where<T1,T2,T3,T4,T5>& w);

  // Size methods.
  virtual int size(const vbl_basic_relation_where<T1,T2,T3,T4,T5>& w);
  virtual bool empty(const vbl_basic_relation_where<T1,T2,T3,T4,T5>& w);

  // Get pointer to this for downcasts.
  virtual void* This();

protected:
  // Really add a tuple.
  bool DoInsert(const vbl_basic_tuple<T1,T2,T3,T4,T5>& t);

protected:
  first_map i1;
  second_map i2;
  third_map i3;
  fourth_map i4;
  fifth_map i5;

  bool u1;
  bool u2;
  bool u3;
  bool u4;
  bool u5;
};

template <class T1, class T2, 
  VCL_DFL_TYPE_PARAM_STLDECL(T3,vbl_basic_optional), 
  VCL_DFL_TYPE_PARAM_STLDECL (T4, vbl_basic_optional), 
  VCL_DFL_TYPE_PARAM_STLDECL(T5,vbl_basic_optional)>
class vbl_br_default_factory : public vbl_basic_relation_factory<T1,T2,T3,T4,T5>
{
public:
  virtual ~vbl_br_default_factory();
  virtual vbl_br_impl<T1,T2,T3,T4,T5>* Generate(vcl_string name);
  virtual void CheckEmpty(vbl_br_impl<T1,T2,T3,T4,T5>* impl);
};

#endif
