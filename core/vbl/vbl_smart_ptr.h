#ifndef vbl_smart_ptr_h_
#define vbl_smart_ptr_h_
// .NAME vbl_smart_ptr - A templated smart pointer class
// .INCLUDE vbl/vbl_smart_ptr.h
// .FILE vbl_smart_ptr.cxx
// .EXAMPLE vbl/examples/vbl_smart_ptr_example.cxx
//
// .SECTION Description
// This class requires that the class being templated over has
// the following signatures (methods) :
//   void T::ref();
//   void T::unref();
//
// By default, the vbl_smart_ptr<T> will ref() the object given
// to it upon construction and unref() it upon destruction. In
// some cases, however, it is useful to cause an unref() immediately
// and to avoid an unref() in the constructor. For example, in the
// cyclic data structure
//
// start -> A -> B -> C -> D -> E
//          ^                   |
//          |                   |
//          +-------------------+
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
//
// .SECTION See also
//   vbl_refcount
//
// .SECTION Author
//   Richard Hartley (original Macro version),
//   William A. Hoffman (current templated version)
//
// .SECTION Modifications
// 2000.05.15 François BERTEL Added some missing <T>
// 2000.05.16 Peter Vanroose  Operators > < >= <= made const
// 2000.09.13 fsm@robots      Added rationale for unprotect().

#include <vcl/vcl_iosfwd.h>

template <class T>
class vbl_smart_ptr {
public:
  vbl_smart_ptr ()
    : protected_(true), ptr_(0) { }

  vbl_smart_ptr (vbl_smart_ptr<T> const &p)
    : protected_(true), ptr_(p.as_pointer()) { if (ptr_) ref(ptr_); }

  vbl_smart_ptr (T *p)
    : protected_(true), ptr_(p) { if (ptr_) ref(ptr_); }

  ~vbl_smart_ptr () {
    // the strange order of events in this function is to avoid
    // heap corruption if unref() causes *this to be deleted.
    if (protected_) {
      T *old_ptr = ptr_;
      ptr_ = 0;
      if (old_ptr)
	unref(old_ptr);
    }
    else
      ptr_ = 0;
  }

  //: Assignment
  vbl_smart_ptr<T> &operator = (vbl_smart_ptr<T> const &r) { return operator=(r.as_pointer()); }
  vbl_smart_ptr<T> &operator = (T *r) {
    if (ptr_ != r) {
      // If there are circular references, calling unref() may
      // cause *this to be destroyed and so assigning to 'ptr_'
      // would be ill-formed and could cause heap corruption.
      // Hence perform the unref() only at the very end.
      T *old_ptr = ptr_;
      ptr_ = r;

      if (ptr_)
	ref(ptr_);

      // *this might get deleted now, but that's ok.
      if (old_ptr)
	unref(old_ptr);
    }
    return *this;
  }

  //: Cast to bool
  operator bool () const { return ptr_ != (T*)0; }

  //: Dereferencing the pointer
  T &operator * () const { return *ptr_; }

  //: These methods all return the raw/dumb pointer.
  // WARNING : Do not add an automatic cast to T*.
  //           This is intrinsically incorrect as you loose the smartness!
  //           In cases where you really need the pointer, it is better
  //           to be explicit about it and use one of the methods.
  T *operator -> () const { return ptr_; }

  T *ptr () const { return ptr_; }

  T *as_pointer () const { return ptr_; }

  //: Used for breaking circular references (see above).
  void unprotect() {
    if (protected_ && ptr_)
      unref(ptr_);
    protected_ = false;
  }

  //  //: If a T_ref is converted to a pointer then back to a T_ref,
  //  // you'll need to call this
  //  void protect() { ref(); }

  //: Relational operators.
  // [There's no need for casts to void* or any other pointer type than T* here.]
  bool operator == (T const *p) const { return ptr_ == p; }
  bool operator == (vbl_smart_ptr<T> const &p) const { return ptr_ == p.as_pointer(); }
  bool operator <  (vbl_smart_ptr<T> const &p) const { return ptr_ <  p.as_pointer(); }
  bool operator >  (vbl_smart_ptr<T> const &p) const { return ptr_ >  p.as_pointer(); }
  bool operator <= (vbl_smart_ptr<T> const &p) const { return ptr_ <= p.as_pointer(); }
  bool operator >= (vbl_smart_ptr<T> const &p) const { return ptr_ >= p.as_pointer(); }

private:
  // These two methods should not be inlined as they call T's ref()
  // and unref() or are specializations. The big gain from that is
  // that vbl_smart_ptr<T> can be forward declared even if T is still
  // an incomplete type.
  static void ref  (T *p);
  static void unref(T *p);

  // The protected flag says whether or not the object held will be
  // unref()fed when the smart pointer goes out of scope.
  bool protected_;

  // Pointer to object, or 0.
  T *ptr_;
};


// GCC 2.7 really *does* need this non-member equality operator,
// even though a member version is already present.
#if defined(VCL_GCC_27)
template <class T>
inline bool operator== (vbl_smart_ptr<T> const& a, vbl_smart_ptr<T> const& b)
{
  return a.as_pointer() == b.as_pointer();
}
#endif

template <class T>
inline bool operator== (T const* p, vbl_smart_ptr<T> const& a)
{
  return a.as_pointer() == p;
}

// SGI CC thinks that "base_ref == 0" is a comparison with int.
// Try to not use that specific comparison; use "!base_ref" instead,
// as cast-to-bool is provided. - PVr
#if defined(VCL_SGI_CC) && 0
template <class T>
inline bool operator== (vbl_smart_ptr<T> const& a, int p)
{
  return a.as_pointer() == (T*)p;
}
#endif

// Sunpro and GCC need an ostream operator. It need not be inline
// because if you're about to make a system call you can afford the
// cost of a function call.
template <class T>
ostream& operator<< (ostream&, vbl_smart_ptr<T> const&);

#define VBL_SMART_PTR_INSTANTIATE(T) \
extern "please include vbl/vbl_smart_ptr.txx instead"

#endif // vbl_smart_ptr_h_
