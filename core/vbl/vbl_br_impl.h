// This is core/vbl/vbl_br_impl.h
#ifndef vbl_br_impl_h
#define vbl_br_impl_h
//:
// \file
// \author Rupert Curwen, GE Corporate Research and Development
// \date   Jan 14th, 1998
//
// \verbatim
// Modifications
// PDA (Manchester) 21/03/2001: Tidied up the documentation
// IMS (Manchester) 21/10/2003: Deprecated - Decision at Providence 2002 Meeting.
//                              Can be deleted after VXL-1.1
// \endverbatim


#include <vcl_string.h>
#include <vcl_hash_string.h>

#include <vbl/vbl_hash_map.h>
#include <vbl/vbl_basic_optional.h>
#include <vbl/vbl_basic_tuple.h>
#include <vbl/vbl_basic_relation_where.h>
#include <vbl/vbl_ref_count.h>
#include <vcl_deprecated_header.h>

template <class T1, class T2,
          VCL_DFL_TYPE_PARAM_STLDECL(T3, vbl_basic_optional),
          VCL_DFL_TYPE_PARAM_STLDECL(T4, vbl_basic_optional),
          VCL_DFL_TYPE_PARAM_STLDECL(T5, vbl_basic_optional)>
class vbl_basic_relation_factory;

//------------------------------------------------------------
//: Abstract base class for implementations of easy relation iterators.
//
// \deprecated Because no-one knows what it does.
template <class T1, class T2,
          VCL_DFL_TYPE_PARAM_STLDECL(T3,vbl_basic_optional),
          VCL_DFL_TYPE_PARAM_STLDECL(T4,vbl_basic_optional),
          VCL_DFL_TYPE_PARAM_STLDECL(T5,vbl_basic_optional)>
class vbl_br_iter_impl
{
 public:
  typedef vbl_basic_tuple<T1,T2,T3,T4,T5> tuple;
  typedef vbl_basic_relation_where<T1,T2,T3,T4,T5> where_clause;

 public:
  //: Constructor.
  vbl_br_iter_impl() {}

  //: Copy constructor.
  vbl_br_iter_impl(const vbl_br_iter_impl<T1,T2,T3,T4,T5>&) {}

  //: Destructor.
  virtual ~vbl_br_iter_impl() {}

  //: Make a copy of this instance.
  virtual vbl_br_iter_impl<T1,T2,T3,T4,T5>* copy() const = 0;

  //: Dereference.
  virtual vbl_basic_tuple<T1,T2,T3,T4,T5>* deref() const = 0;

  //: Increment.
  virtual void incr() = 0;

  //: Compare.
  virtual bool compare(const vbl_br_iter_impl<T1,T2,T3,T4,T5>& i) const = 0;
};

//------------------------------------------------------------
//: Abstract base class for implementations of easy relations.
// vbl_br_impl provides the implementation for the
// vbl_basic_relation.  Sub-class from this class to provide
// specialized implementations for specific types, and use the
// factory mechanism to make your implementation the default used
// for those types.  This is an abstract base class, the only
// functionality implemented here being the naming of
// relations.
//
// \deprecated Because no-one knows what it does.
template <class T1, class T2,
          VCL_DFL_TYPE_PARAM_STLDECL(T3,vbl_basic_optional),
          VCL_DFL_TYPE_PARAM_STLDECL (T4, vbl_basic_optional),
          VCL_DFL_TYPE_PARAM_STLDECL(T5,vbl_basic_optional)>
class vbl_br_impl : public vbl_ref_count
{
 protected:
  vcl_string name;
 public:
  typedef vbl_basic_relation_where<T1,T2,T3,T4,T5> where_clause;
  typedef vbl_br_iter_impl<T1,T2,T3,T4,T5> iterator;

 public:
  //: Constructor.
  vbl_br_impl(const vcl_string& n) : name(n) {}

  //: Copy constructor.
  vbl_br_impl(const vbl_br_impl<T1,T2,T3,T4,T5>& i) : name(i.name) {}

  //: Destructor.
  virtual ~vbl_br_impl() {}

  //: Get the factory for this class.
  static vbl_basic_relation_factory<T1,T2,T3,T4,T5>* GetFactory();

  //: Set the factory for this class.
  static void SetFactory(vbl_basic_relation_factory<T1,T2,T3,T4,T5>* f);

  //: Add a tuple.
  virtual bool Insert(const vbl_basic_tuple<T1,T2,T3,T4,T5>&) { return false; }

  //: Remove a tuple.
  virtual bool Remove(const vbl_basic_tuple<T1,T2,T3,T4,T5>&) { return false; }

  //: Remove tuples matching where clause.
  virtual bool Remove(const vbl_basic_relation_where<T1,T2,T3,T4,T5>&)
  { return false; }

  //: Make attributes unique.
  virtual bool FirstUnique() { return false; }
  virtual bool SecondUnique() { return false; }
  virtual bool ThirdUnique() { return false; }
  virtual bool FourthUnique() { return false; }
  virtual bool FifthUnique() { return false; }
  virtual void SetFirstUnique(bool) {}
  virtual void SetSecondUnique(bool) {}
  virtual void SetThirdUnique(bool) {}
  virtual void SetFourthUnique(bool) {}
  virtual void SetFifthUnique(bool) {}

  // Get iterators.
  virtual vbl_br_iter_impl<T1,T2,T3,T4,T5>*
    begin(const vbl_basic_relation_where<T1,T2,T3,T4,T5>&) { return NULL; }
  virtual vbl_br_iter_impl<T1,T2,T3,T4,T5>*
    end(const vbl_basic_relation_where<T1,T2,T3,T4,T5>&) { return NULL; }

  // Size methods.
  virtual int size(const vbl_basic_relation_where<T1,T2,T3,T4,T5>&)
  { return 0; }
  virtual bool empty(const vbl_basic_relation_where<T1,T2,T3,T4,T5>&)
  { return true; }

  // Get name.
  const vcl_string& GetName() const { return name; }

  // Get pointer to this.
  virtual void* This() { return NULL; }

 protected:
  static vbl_basic_relation_factory<T1,T2,T3,T4,T5>*& factory();
};

template <class T1, class T2, class T3, class T4, class T5>
class vbl_basic_relation_factory
// defaults for T3 T4 T5 already declared around line 80 -- PLEASE don't put them back here!! - PVr
{
 public:
  typedef vbl_br_impl<T1,T2,T3,T4,T5> implementation;
  typedef vbl_hash_map<vcl_string, void*> existing_map;

 public:
  virtual ~vbl_basic_relation_factory() {};
  virtual vbl_br_impl<T1,T2,T3,T4,T5>* Generate(vcl_string) { return NULL; }
  virtual void CheckEmpty(vbl_br_impl<T1,T2,T3,T4,T5>*) {}

 protected:
  existing_map existing;
};

#endif // vbl_br_impl_h
