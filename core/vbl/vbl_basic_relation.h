// This is core/vbl/vbl_basic_relation.h
#ifndef vbl_basic_relation_h
#define vbl_basic_relation_h
//:
// \file
// \author Rupert Curwen, GE Corporate Research and Development
// \date   31/10/1997
//
// \verbatim
// Modifications
// PDA (Manchester) 21/03/2001: Tidied up the documentation
//   Feb.2002 - Peter Vanroose - brief doxygen comment placed on single line
// \endverbatim

#include <vcl_string.h>
#include <vcl_cassert.h>
#include <vcl_iosfwd.h>
#include <vbl/vbl_basic_optional.h>
#include <vbl/vbl_basic_tuple.h>
#include <vbl/vbl_br_impl.h>
#include <vbl/vbl_basic_relation_type.h>

#define BR_DEFAULT1 VCL_DFL_TMPL_ARG(vbl_basic_optional)
#define BR_DEFAULT2 BR_DEFAULT1 VCL_DFL_TMPL_ARG(vbl_basic_optional)
#define BR_DEFAULT3 BR_DEFAULT2 VCL_DFL_TMPL_ARG(vbl_basic_optional)

#ifndef vbl_basic_relation_where_h
template <class T1, class T2,
  VCL_DFL_TYPE_PARAM_STLDECL(T3,vbl_basic_optional),
  VCL_DFL_TYPE_PARAM_STLDECL(T4,vbl_basic_optional),
  VCL_DFL_TYPE_PARAM_STLDECL(T5,vbl_basic_optional)>
class vbl_basic_relation_where;
#endif

//------------------------------------------------------------
//: vbl_basic_relation iterator.
// Holds a sub-class of vbl_br_iter_impl which does the implementation.
template <class T1, class T2,
  VCL_DFL_TYPE_PARAM_STLDECL(T3,vbl_basic_optional),
  VCL_DFL_TYPE_PARAM_STLDECL(T4,vbl_basic_optional),
  VCL_DFL_TYPE_PARAM_STLDECL(T5,vbl_basic_optional)>
class vbl_basic_relation_iterator
{
 public:
  typedef vbl_basic_tuple<T1,T2,T3,T4,T5> tuple;
  typedef vbl_basic_relation_iterator<T1,T2,T3,T4,T5> iterator;
  typedef vbl_br_iter_impl<T1,T2,T3,T4,T5> implementation;

  vbl_basic_relation_iterator();
  vbl_basic_relation_iterator(vbl_br_iter_impl<T1,T2,T3,T4,T5>* im);
  vbl_basic_relation_iterator(vbl_basic_relation_iterator<T1,T2,T3,T4,T5> const& it);
  ~vbl_basic_relation_iterator();

  tuple& operator*() const
  { tuple* t = impl->deref(); assert(t != 0); return *t; }

  //: Prefix increment
  iterator& operator++() { impl->incr(); return *this; }
  //: Postfix increment
  iterator operator++(int) { iterator tmp(*this); impl->incr(); return tmp; }

  inline bool operator==(const iterator& it) const
  { return impl->compare(*it.impl); }
  inline bool operator!=(const iterator& it) const
  { return ! impl->compare(*it.impl); }

  vbl_basic_relation_iterator<T1,T2,T3,T4,T5>&
    operator=(vbl_basic_relation_iterator<T1,T2,T3,T4,T5> const& it);

 protected:
  implementation* impl;
};

//------------------------------------------------------------
//: An easy-to-use relation for holding objects.
// Holds a sub-class of vbl_br_impl which does the implementation.
// The relation is templated, and accepts varying number of
// classes, from one to five.  This restriction of up to five
// attributes per tuple may be increased as needed.
//
template <class T1, class T2,
  VCL_DFL_TYPE_PARAM_STLDECL(T3,vbl_basic_optional),
  VCL_DFL_TYPE_PARAM_STLDECL (T4, vbl_basic_optional),
  VCL_DFL_TYPE_PARAM_STLDECL(T5,vbl_basic_optional)>
class vbl_basic_relation : public vbl_basic_relation_type
{
  friend class vbl_basic_relation_iterator<T1,T2,T3,T4,T5>;

 public:
  typedef vbl_basic_tuple<T1,T2,T3,T4,T5> tuple;
  typedef vbl_basic_relation_iterator<T1,T2,T3,T4,T5> iterator;
  typedef vbl_br_impl<T1,T2,T3,T4,T5> implementation;
  typedef vbl_basic_relation_where<T1,T2,T3,T4,T5> where_clause;

 public:
  //: Constructor.
  vbl_basic_relation(const vcl_string& name);

  //: Copy constructor.
  vbl_basic_relation(vbl_basic_relation<T1,T2,T3,T4,T5> const& rel);

  //: Destructor.
  ~vbl_basic_relation();

  //: Add a tuple.
  bool insert(T1 t1,T2 t2) { return impl->Insert(tuple(t1,t2)); }
  bool insert(T1 t1,T2 t2,T3 t3) { return impl->Insert(tuple(t1,t2,t3)); }
  bool insert(T1 t1,T2 t2,T3 t3,T4 t4)
  { return impl->Insert(tuple(t1,t2,t3,t4)); }
  bool insert(T1 t1,T2 t2,T3 t3,T4 t4,T5 t5)
  { return impl->Insert(tuple(t1,t2,t3,t4,t5)); };
  bool insert(const tuple& t) { return impl->Insert(t); }

  //: Remove a tuple.
  bool remove(T1 t1,T2 t2) { return impl->Remove(tuple(t1,t2)); }
  bool remove(T1 t1,T2 t2,T3 t3) { return impl->Remove(tuple(t1,t2,t3)); }
  bool remove(T1 t1,T2 t2,T3 t3,T4 t4)
  { return impl->Remove(tuple(t1,t2,t3,t4)); }
  bool remove(T1 t1,T2 t2,T3 t3,T4 t4,T5 t5)
  { return impl->Remove(tuple(t1,t2,t3,t4,t5)); }
  bool remove(const tuple& t) { return impl->Remove(t); }

  //: Iterators.
  iterator begin();
  iterator end();

  //: Size queries.
  int size() { return impl->size(*where); }
  bool empty() { return impl->empty(*where); }

  //: Clear.
  bool clear();

  //: Get name.
  const vcl_string& get_name() const { return impl->GetName(); }

  // Remove based on attribute.
  bool remove_first(T1 t1);
  bool remove_second(T2 t2);
  bool remove_third(T3 t3);
  bool remove_fourth(T4 t4);
  bool remove_fifth(T5 t5);

  // Select on an attribute.
  vbl_basic_relation<T1,T2,T3,T4,T5> where_first(T1 t1);
  vbl_basic_relation<T1,T2,T3,T4,T5> where_second(T2 t2);
  vbl_basic_relation<T1,T2,T3,T4,T5> where_third(T3 t3);
  vbl_basic_relation<T1,T2,T3,T4,T5> where_fourth(T4 t4);
  vbl_basic_relation<T1,T2,T3,T4,T5> where_fifth(T5 t5);
  vbl_basic_relation<T1,T2,T3,T4,T5>
    where_is(const vbl_basic_relation_where<T1,T2,T3,T4,T5>& where);

  // Make attributes unique.
  bool first_unique() { return impl->FirstUnique(); }
  bool second_unique() { return impl->SecondUnique(); }
  bool third_unique() { return impl->ThirdUnique(); }
  bool fourth_unique() { return impl->FourthUnique(); }
  bool fifth_unique() { return impl->FifthUnique(); }
  void set_first_unique(bool b) { impl->SetFirstUnique(b); }
  void set_second_unique(bool b) { impl->SetSecondUnique(b); }
  void set_third_unique(bool b) { impl->SetThirdUnique(b); }
  void set_fourth_unique(bool b) { impl->SetFourthUnique(b); }
  void set_fifth_unique(bool b) { impl->SetFifthUnique(b); }

  // Print the relation on cout.
  inline void print_relation() const { dump_relation(vcl_cout); }

  // Print the relation on cerr.
  void dump_relation(vcl_ostream& str = vcl_cerr) const;

  // Type downcasting methods.
  virtual void* get_type();
  virtual void* get_this();
  static void* get_static_type();
  static vbl_basic_relation<T1,T2,T3,T4,T5>* cast(vbl_basic_relation_type* r);

 protected:
  implementation* impl;
  where_clause* where;
};


template
 <class T1,
  class T2,
  class T3,
  class T4,
  class T5>
inline vcl_ostream& operator<<(vcl_ostream& str,
                               vbl_basic_relation<T1,T2,T3,T4,T5>& rel)
{
  rel.dump_relation(str);
  return str;
}

#endif // vbl_basic_relation_h
