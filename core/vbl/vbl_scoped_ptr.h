#ifndef vbl_scoped_ptr_h_
#define vbl_scoped_ptr_h_
//:
// \file
// \author Amitha Perera
// \brief Scoped pointer lifted from BOOST.
//
//  (C) Copyright Greg Colvin and Beman Dawes 1998, 1999.
//  Copyright (c) 2001, 2002 Peter Dimov
//
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.
//
//  http://www.boost.org/libs/smart_ptr/scoped_ptr.htm
//
//  Modified from the original boost sources to fit the VXL restrictions.

#include <vcl_compiler.h>
#include <vbl/vbl_checked_delete.h>

//:
//  vbl_scoped_ptr mimics a built-in pointer except that it guarantees
//  deletion of the object pointed to, either on destruction of the
//  vbl_scoped_ptr or via an explicit reset(). vbl_scoped_ptr is a
//  simple solution for simple needs; use vbl_shared_ptr or
//  std::auto_ptr if your needs are more complex.
//
//  To use this to manage pointer member variables using forward
//  declaration, explicitly define a destructor in your .cxx so that
//  the vbl_scoped_ptr destructor is called there rather than being
//  inlined. For example, Y.h:
//  \code
//    struct X;
//    struct Y {
//      vbl_scoped_ptr<X> member;
//
//      ~Y() { } // NO: causes ~vbl_scoped_ptr<X> to be instantated, which means X must be complete.
//      ~Y(); // YES: destructor not yet generated
//    };
//  \endcode
//  Y.cxx:
//  \code
//    #include "X.h"
//    Y::~Y()
//    { } // causes ~vbl_scoped_ptr<X> to be instantiated and inlined, but X is complete here, so all is well.
//  \endcode

template <class T>
class vbl_scoped_ptr
{
 private:
  T* ptr_;

  // not copyable, not assignable.
  vbl_scoped_ptr( vbl_scoped_ptr const& );
  vbl_scoped_ptr& operator=( vbl_scoped_ptr const& );

  typedef vbl_scoped_ptr<T> this_type;

  VCL_SAFE_BOOL_DEFINE;

 public:
  typedef T element_type;

  //:
  explicit vbl_scoped_ptr( T* p = 0 )
    : ptr_(p) // never throws
  {
  }

  //:
  // T must be complete when this destructor is instantiated.
  ~vbl_scoped_ptr() // never throws
  {
    vbl_checked_delete(ptr_);
  }

  //: Make this own \arg p, releasing any existing pointer.
  void reset( T* p = 0 ) // never throws
  {
    this_type(p).swap(*this);
  }

  //:
  T& operator*() const // never throws
  {
    return *ptr_;
  }

  //:
  T* operator->() const // never throws
  {
    return ptr_;
  }

  //:
  T* get_pointer() const // never throws
  {
    return ptr_;
  }

    // implicit conversion to "bool"

  //: Safe implicit conversion to bool.
  //
  // This allows for if(sp) type of usage.
  operator safe_bool () const
  {
    return ptr_ ? VCL_SAFE_BOOL_TRUE : 0;
  }

  //:
  bool operator! () const // never throws
  {
    return ptr_ == 0;
  }

  //:
  void swap( vbl_scoped_ptr& b ) // never throws
  {
    T* tmp = b.ptr_;
    b.ptr_ = ptr_;
    ptr_ = tmp;
  }
};

#endif // vbl_scoped_ptr_h_
