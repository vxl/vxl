#ifndef _SMART_PTR_H_
#define _SMART_PTR_H_

//-----------------------------------------------------------------------------
//
// .NAME vbl_smart_ptr - A templated smart pointer class.
// .LIBRARY vbl
// .HEADER  Basics Package
// .INCLUDE vbl/smart_ptr.h
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
//
// .SECTION Modifications
//   
//
//-----------------------------------------------------------------------------

//
// Builds off the vbl_refcount to give smart pointers
//

//--------------------------------------------------------------------

#ifdef _HERE_IS_AN_EXAMPLE_
/*
	// Here is an example of how to make a smart pointer class.
	// A smart pointer can be made from any class that subclasses off
	// vbl_ref_count.  For example, image.
	// Note : This code is just for example, and is commented out

	typedef vbl_smart_ptr<vil_image> vil_image_ref;

	// To instantiate create a .cxx file in the Templates directory of
	// the class you want the smart pointer of.  In the file do this:

	// start vbl_smart_ptr+vil_image-.C 
	#include <vil/vil_image.h>
	#include <vbl/vbl_smart_ptr.h>

	VBL_SMART_PTR_INSTANTIATE(vil_image);
	// end vbl_smart_ptr+vil_image-.C 

	// If you use smart pointers, then you should not get memory leaks
*/
#endif

#include <vcl/vcl_iostream.h>

template <class T>
class vbl_smart_ptr 
{
private:

  bool protected_;	
  T* ptr_;

public:

  vbl_smart_ptr () 
  {
    protected_ = true;
    ptr_ = 0;
  }

  vbl_smart_ptr (const vbl_smart_ptr<T> &p)
  { 
    protected_ = true;
    ptr_ = p.ptr_; 
    ref(); 
  }
  
  vbl_smart_ptr (T *p)
  { 
    protected_ = true;
    ptr_ = p; 
    ref(); 
  }				

  ~vbl_smart_ptr ()
  {
    unref();
  }

  T *operator -> () const
  { 
    return ptr_; 
  }

  T *ptr () const 
  { 
    // This returns the pointer.  
    return ptr_; 
  }

  // -- used for getting around circular references
  void unprotect()
  {
    unref();
  }

  // Comparison of pointers
  bool operator < (const vbl_smart_ptr &r)
  { return (void*)ptr_ < (void*) r.ptr_; }

  bool operator > (const vbl_smart_ptr &r)
  { return (void*)ptr_ > (void*) r.ptr_; }

  bool operator <= (const vbl_smart_ptr &r)
  { return (void*)ptr_ <= (void*) r.ptr_; }

  bool operator >= (const vbl_smart_ptr &r)
  { return (void*)ptr_ >= (void*) r.ptr_; }

  vbl_smart_ptr &operator = (const vbl_smart_ptr &r)
  { 
    return this->operator = (r.ptr()); 
  }
  
  vbl_smart_ptr &operator = (T *r)
  {								 
    if (ptr_ != r)
      {
        unref();
        ptr_ = r;
        ref();
      }
    return *this;
  }

  // Dereferencing the pointer
  T &operator * () const { return *ptr_; }

  // Cast to T * 
  operator T * () const { return ptr_; }

private:

  void ref();
  void unref();
};


template <class T>
inline bool operator == (vbl_smart_ptr<T> const &a, vbl_smart_ptr<T> const &b)
{
  return a.ptr() == b.ptr();
}

// Sunpro and GCC need an ostream operator.
template <class T>
inline ostream& operator<< (ostream& os, vbl_smart_ptr<T> const& r)
{
  return os << r.ptr();
}

#define VBL_SMART_PTR_INSTANTIATE(T) "error, see .txx file"

#endif
