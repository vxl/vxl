// This is mul/mbl/mbl_cloneable_ptr.h
#ifndef mbl_cloneable_ptr_h
#define mbl_cloneable_ptr_h
//:
//  \file

#include <vsl/vsl_binary_loader.h>
#include <vcl_cassert.h>

//=======================================================================
//: Cunning pointer for objects that can be cloned.
//  Used to record base class pointers to objects
//  When copied, the object pointed to gets cloned.
//  When written or read to/from binary streams,
//  suitable polymorphic I/O is invoked.
template <class BaseClass>
class mbl_cloneable_ptr
{
  BaseClass* ptr_;
 public:
  //: Default constructor (zeros pointer)
  mbl_cloneable_ptr() : ptr_(0) {}

  //: Delete object pointed to and set pointer to zero
  void deleteObject() { delete ptr_; ptr_=0; }

  //: Destructor
  ~mbl_cloneable_ptr() { deleteObject(); }

  //: Copy constructor
  mbl_cloneable_ptr(const mbl_cloneable_ptr<BaseClass>& p) : ptr_(0) { *this = p; }

  //: Copy operator
  mbl_cloneable_ptr<BaseClass>& operator=(const mbl_cloneable_ptr<BaseClass>& p)
  {
    if (this==&p) return *this;
    deleteObject(); if (p.ptr_!=0) ptr_=p.ptr_->clone();
    return *this;
  }

  //: Copy operator - takes clone of p
  mbl_cloneable_ptr<BaseClass>& operator=(const BaseClass& p)
  {
    if (ptr_==&p) return *this;
    deleteObject();
    ptr_= p.clone();
    return *this;
  }

  //: Copy operator - takes responsibility for *p
  //  Sets internal pointer to p, and takes responsibility
  //  for deleting *p
  mbl_cloneable_ptr<BaseClass>& operator=(BaseClass* p)
  {
    if (ptr_==p) return *this;
    deleteObject();
    ptr_= p;
    return *this;
  }

  //: Return true if pointer defined
  bool isDefined() const { return ptr_!=0; }

  //: Make object behave like pointer to BaseClass
  const BaseClass* operator->() const { return ptr_; }

  //: Make object behave like pointer to BaseClass
  BaseClass* operator->() { return ptr_; }

  //: Return actual pointer
  const BaseClass* ptr() const { return ptr_; }

  //: Cast to allow object to look like thing pointed to
  operator BaseClass&() { assert(ptr_!=0); return *ptr_; }

  //: Dereferencing the pointer
  BaseClass &operator * () { return *ptr_; }

  //: Dereferencing the pointer
  const BaseClass &operator * () const { return *ptr_; }

  //: Cast to allow object to look like thing pointed to
  operator const BaseClass&() const { assert(ptr_!=0); return *ptr_; }

  //: Save to binary stream
  void b_write(vsl_b_ostream& bfs) const
  {
    vsl_b_write(bfs,ptr_);
  }

  //: Load from binary stream
  void b_read(vsl_b_istream& bfs)
  {
    deleteObject();
    vsl_b_read(bfs,ptr_);
  }
};

template <class BaseClass>
void vsl_b_write(vsl_b_ostream& bfs, const mbl_cloneable_ptr<BaseClass>& p)
{ p.b_write(bfs);  }

template <class BaseClass>
void vsl_b_read(vsl_b_istream& bfs, mbl_cloneable_ptr<BaseClass>& p)
{ p.b_read(bfs);  }

#define MBL_CLONEABLE_PTR_INSTANTIATE(T) \
template class mbl_cloneable_ptr< T >; \
template void vsl_b_write(vsl_b_ostream& bfs, const mbl_cloneable_ptr< T >& p);\
template void vsl_b_read(vsl_b_istream& bfs, mbl_cloneable_ptr< T >& p)

#endif  // mbl_cloneable_ptr_h
