// This is core/vil/vil_smart_ptr.h
#ifndef vil_smart_ptr_h_
#define vil_smart_ptr_h_
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
// 2002.9.20  Ian Scott       Copied into vil1, renamed and simplified.
// \endverbatim

#include <vcl_iosfwd.h>

//: A templated smart pointer class
// This class requires that the class being templated over has
// the following signatures (methods) :
// \code
//   void T::ref();
//   void T::unref();
// \endcode
//
//
// See also vbl_ref_count
template <class T>
class vil_smart_ptr
{
  //: Helper types for safe boolean conversion.
  struct safe_bool_dummy { void dummy() {} };
  typedef void (safe_bool_dummy::* safe_bool)();
 public:
  vil_smart_ptr ()
    :  ptr_(0) { }

  vil_smart_ptr (vil_smart_ptr<T> const &p)
    :  ptr_(p.as_pointer()) { if (ptr_) ref(ptr_); }

  vil_smart_ptr (T *p)
    :  ptr_(p) { if (ptr_) ref(ptr_); }

  ~vil_smart_ptr ()
  {
    // the strange order of events in this function is to avoid
    // heap corruption if unref() causes *this to be deleted.
    T *old_ptr = ptr_;
    ptr_ = 0;
    if (old_ptr)
      unref(old_ptr);
  }

  //: Assignment
  vil_smart_ptr<T> &operator = (vil_smart_ptr<T> const &r)
  {
    return operator=(r.as_pointer());
  }

  vil_smart_ptr<T> &operator = (T *r)
  {
    if (ptr_ != r)
    {
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
  operator safe_bool () const
    { return (ptr_ != (T*)0)? &safe_bool_dummy::dummy : 0; }

  //: Inverse bool
  bool operator!() const
    { return (ptr_ != (T*)0)? false : true; }

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
  bool operator==(vil_smart_ptr<T>const&p)const{return ptr_ == p.as_pointer();}

  //: Do a shallow inequality
  // Do the smart pointers not point to the same object.
  bool operator!=(vil_smart_ptr<T>const&p)const{return ptr_ != p.as_pointer();}
  bool operator< (vil_smart_ptr<T>const&p)const{return ptr_ <  p.as_pointer();}
  bool operator> (vil_smart_ptr<T>const&p)const{return ptr_ >  p.as_pointer();}
  bool operator<=(vil_smart_ptr<T>const&p)const{return ptr_ <= p.as_pointer();}
  bool operator>=(vil_smart_ptr<T>const&p)const{return ptr_ >= p.as_pointer();}

 private:
  // These two methods should not be inlined as they call T's ref()
  // and unref() or are specializations. The big gain from that is
  // that vil_smart_ptr<T> can be forward declared even if T is still
  // an incomplete type.
  static void ref  (T *p);
  static void unref(T *p);

  //: Pointer to object, or 0.
  T *ptr_;
};


//: Comparison of pointer with smart-pointer (cannot be a member function)
template <class T>
inline bool operator== (T const* p, vil_smart_ptr<T> const& a)
{
  return a.as_pointer() == p;
}

template <class T>
inline bool operator!= (T const* p, vil_smart_ptr<T> const& a)
{
  return a.as_pointer() != p;
}

// Work-around for Borland and safe_bool.
#ifdef VCL_BORLAND
template <class T>
inline
bool operator&&(const vil_smart_ptr<T>& ptr, bool b)
{
  return b && (ptr?true:false);
}
template <class T>
inline
bool operator&&(bool b, const vil_smart_ptr<T>& ptr)
{
  return b && (ptr?true:false);
}
template <class T>
inline
bool operator||(const vil_smart_ptr<T>& ptr, bool b)
{
  return b || (ptr?true:false);
}
template <class T>
inline
bool operator||(bool b, const vil_smart_ptr<T>& ptr)
{
  return b || (ptr?true:false);
}
#endif

// Sunpro and GCC need a vcl_ostream operator. It need not be inline
// because if you're about to make a system call you can afford the
// cost of a function call.
template <class T>
vcl_ostream& operator<< (vcl_ostream&, vil_smart_ptr<T> const&);

#define VIL_SMART_PTR_INSTANTIATE(T) \
extern "please include vil/vil_smart_ptr.txx instead"

#endif // vil_smart_ptr_h_
