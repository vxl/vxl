#ifndef vbl_smart_ptr_h_
#define vbl_smart_ptr_h_
// .NAME vbl_smart_ptr - A templated smart pointer class.
// .FILE vbl/smart_ptr.cxx
// .EXAMPLE vbl/examples/vbl_smart_ptr_example.cxx
//
// .SECTION Description
//   This class requires that the class being templated over has 
// a unref and ref method, and the stream operator<< function.  Currently,
// it has only been tested with vbl_refcount.
// 
// .SECTION See also
//   vbl_refcount
//
// .SECTION Author
//   Richard Hartley (original Macro version), 
//   William A. Hoffman (current templated version)
//   fsm@robots: eliminated out-of-date documentation and moved
//               example to examples directory.
//
// .SECTION Modifications
// 2000.05.15 François BERTEL Added some missing <T>
// 2000.05.16 Peter Vanroose  Operators > < >= <= made const
//
//-----------------------------------------------------------------------------

//
// Builds off the vbl_refcount to give smart pointers
//

//--------------------------------------------------------------------

#include <vcl/vcl_iostream.h>

template <class T>
class vbl_smart_ptr {
public:
  vbl_smart_ptr () : protected_(true), ptr_(0) { }
  vbl_smart_ptr (vbl_smart_ptr<T> const &p) : protected_(true), ptr_(p.as_pointer()) { ref(); }
  vbl_smart_ptr (T *p) : protected_(true), ptr_(p) { ref(); }
  ~vbl_smart_ptr () { unref(); ptr_ = 0; }
  
  //: Assignment  
  vbl_smart_ptr<T> &operator = (vbl_smart_ptr<T> const &r) { return operator=(r.as_pointer()); }
  vbl_smart_ptr<T> &operator = (T *r) {
    if (ptr_ != r) { unref(); ptr_ = r; ref(); }
    return *this;
  }
  
  //: Cast to bool
  operator bool () const { return ptr_ != (T*)0; }

  //: Dereferencing the pointer
  T &operator * () const { return *ptr_; }

  //: These return the raw/dumb pointer.
  T *operator -> () const { return ptr_; }
  T *ptr () const { return ptr_; }
  T *as_pointer () const { return ptr_; }
  // WARNING : Please do not make an automatic cast to T*.
  //           This is intrinsically incorrect as you loose the smartness!
  //           In cases where you really need the pointer, use .as_pointer()
  //DO_NOT_USE operator T* () const { return ptr_; }

  //: Used for getting around circular references
  void unprotect() { unref(); }

  // Comparison of pointers
  bool operator == (T const *p) const { return ptr_ == p; }
  bool operator == (vbl_smart_ptr<T> const &p) const { return ptr_ == p.as_pointer(); }
  //fsm: what's all the void *s for?
  //answer: this is pointer arithmetic, so there should *not* be any void* - PVr
  bool operator <  (vbl_smart_ptr<T> const &r) const { return ptr_ <   r.as_pointer(); }
  bool operator >  (vbl_smart_ptr<T> const &r) const { return ptr_ >   r.as_pointer(); }
  bool operator <= (vbl_smart_ptr<T> const &r) const { return ptr_ <=  r.as_pointer(); }
  bool operator >= (vbl_smart_ptr<T> const &r) const { return ptr_ >=  r.as_pointer(); }

private:
  void ref(); // cannot inline here as this calls T's ref()
  void unref(); // idem
  
  bool protected_;	
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

// Sunpro and GCC need an ostream operator.
template <class T>
inline ostream& operator<< (ostream& os, vbl_smart_ptr<T> const& r)
{
  return os << (void*) r.as_pointer();
}

#define VBL_SMART_PTR_INSTANTIATE(T) \
extern "please include vbl/vbl_smart_ptr.txx instead"

#endif
