// This is core/vbl/vbl_smart_ptr.h
#ifndef vbl_smart_ptr_h_
#define vbl_smart_ptr_h_
//:
// \file
// \brief Contains a templated smart pointer class
// \author Richard Hartley (original Macro version),
//         William A. Hoffman (current templated version)
//
// \verbatim
// Modifications
// 2000.05.15 François BERTEL Added some missing <T>
// 2000.05.16 Peter Vanroose  Operators > < >= <= made const
// 2000.09.13 fsm      Added rationale for unprotect().
// PDA (Manchester) 23/03/2001: Tidied up the documentation
// Peter Vanroose   27/05/2001: Corrected the documentation
//   Feb.2002 - Peter Vanroose - brief doxygen comment placed on single line
// \endverbatim

#include <vcl_iosfwd.h>

// See the documentation in the .txx file about these functions.

template<class T>
void vbl_smart_ptr_ref  (T *p);

template<class T>
void vbl_smart_ptr_unref(T *p);


//: A templated smart pointer class
// This class requires that the class being templated over has
// the following signatures (methods) :
// \code
//   void T::ref();
//   void T::unref();
// \endcode
//
// By default, the vbl_smart_ptr<T> will ref() the object given
// to it upon construction and unref() it upon destruction. In
// some cases, however, it is useful to cause an unref() immediately
// and to avoid an unref() in the constructor. For example, in the
// cyclic data structure
// \code
// start -> A -> B -> C -> D -> E
//          ^                   |
//          |                   |
//          +-------------------+
// \endcode
//
// The refcounts on A, B, C, D, E are 2, 1, 1, 1, 1 so when 'start'
// goes out of scope, the refcount will be 1, 1, 1, 1, 1 and therefore
// the ring never gets deleted. Calling unprotect() on 'E' solves this
// as it effectively transfers ownership of 'A' from 'E' to 'start'.
//
// Although unprotect() can be handy, it should be used with care. It
// can sometimes (but not always) be avoided by assigning 0 (null pointer)
// to one of the nodes in the ring.
//
// See also vbl_ref_count
template <class T>
class vbl_smart_ptr
{
  //: The protected flag says whether or not the object held will be unref()fed when the smart pointer goes out of scope.
  bool protected_;

  //: Pointer to object, or 0.
  T *ptr_;

  VCL_SAFE_BOOL_DEFINE;
 public:
  vbl_smart_ptr ()
    : protected_(true), ptr_(0) { }

  vbl_smart_ptr (vbl_smart_ptr<T> const &p)
    : protected_(true), ptr_(p.as_pointer()) { if (ptr_) vbl_smart_ptr_ref(ptr_); }

  vbl_smart_ptr (T *p)
    : protected_(true), ptr_(p) { if (ptr_) vbl_smart_ptr_ref(ptr_); }

  ~vbl_smart_ptr ()
  {
    // Note that calls to unref will never cause *this to be
    // deleted. It may cause *ptr_ to be deleted.
    if (protected_)
    {
      if (ptr_)
        vbl_smart_ptr_unref(ptr_);
    }
  }

  //: Assignment
  vbl_smart_ptr &operator = (vbl_smart_ptr const &r)
  {
    return operator=(r.as_pointer());
  }

  vbl_smart_ptr &operator = (T *r)
  {
    vbl_smart_ptr_ref(r);
    if (protected_) {
      vbl_smart_ptr_unref(ptr_);
    }
    ptr_ = r; // this must be valid since we incremented the ref count
    protected_ = true;

    return *this;
  }

  //: Cast to bool
   operator safe_bool () const { return ptr_? VCL_SAFE_BOOL_TRUE : 0; }

  //: Inverse boolean value
  bool operator!() const { return ptr_? false : true; }

  //: Dereferencing the pointer
  T &operator * () const { return *ptr_; }

  //: These methods all return the raw/dumb pointer.
  T *operator -> () const { return ptr_; }

  //: These methods all return the raw/dumb pointer.
  T *ptr () const { return ptr_; }

  //: These methods all return the raw/dumb pointer.
  //
  // WARNING : Do not add an automatic cast to T*.
  //           This is intrinsically incorrect as you loose the smartness!
  //           In cases where you really need the pointer, it is better
  //           to be explicit about it and use one of the methods.
  T *as_pointer () const { return ptr_; }

  //: Used for breaking circular references (see above).
  void unprotect() {
    if (protected_ && ptr_)
      vbl_smart_ptr_unref(ptr_);
    protected_ = false;
  }

  //: Is this smart pointer responsible for the object being pointed to
  // If this value is false, the object does not have to save it.
  bool is_protected() const { return protected_;};

#if 0 // no longer needed
  //: If a T_ref is converted to a pointer then back to a T_ref, you'll need to call this
  void protect()
  {
    if (!protected_ && ptr_)
      vbl_smart_ptr_ref(ptr_);
    protected_ = true;
  }
#endif

  // Relational operators.
  //There's no need for casts to void* or any other pointer type than T* here.

  //: Do a shallow equality
  // Do they point to the same object.
  bool operator==(T const *p) const { return ptr_ == p; }

  //: Do a shallow inequality
  // Do the smart pointers not point to the same object.
  bool operator!=(T const *p) const { return ptr_ != p; }

  //: Do a shallow equality
  // Do they point to the same object.
  bool operator==(vbl_smart_ptr<T>const&p)const{return ptr_ == p.as_pointer();}

  //: Do a shallow inequality
  // Do the smart pointers not point to the same object.
  bool operator!=(vbl_smart_ptr<T>const&p)const{return ptr_ != p.as_pointer();}
  bool operator< (vbl_smart_ptr<T>const&p)const{return ptr_ <  p.as_pointer();}
  bool operator> (vbl_smart_ptr<T>const&p)const{return ptr_ >  p.as_pointer();}
  bool operator<=(vbl_smart_ptr<T>const&p)const{return ptr_ <= p.as_pointer();}
  bool operator>=(vbl_smart_ptr<T>const&p)const{return ptr_ >= p.as_pointer();}
};


//: Comparison of pointer with smart-pointer (cannot be a member function)
template <class T>
inline bool operator== (T const* p, vbl_smart_ptr<T> const& a)
{
  return a.as_pointer() == p;
}

template <class T>
inline bool operator!= (T const* p, vbl_smart_ptr<T> const& a)
{
  return a.as_pointer() != p;
}

// Sunpro and GCC need a vcl_ostream operator. It need not be inline
// because if you're about to make a system call you can afford the
// cost of a function call.
template <class T>
vcl_ostream& operator<< (vcl_ostream&, vbl_smart_ptr<T> const&);

#define VBL_SMART_PTR_INSTANTIATE(T) \
extern "please include vbl/vbl_smart_ptr.txx instead"

#endif // vbl_smart_ptr_h_
