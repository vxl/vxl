#ifndef vil_image_ref_h_
#define vil_image_ref_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

#include <vil/vil_generic_image.h>

//: A smart pointer for vil_generic_image
// There should probably be . versions of all the vil_generic_image
// methods. For now, use ->method().
class vil_image_ref {
public:

  vil_image_ref(vil_generic_image *p = 0) : ptr(p) {
    if (ptr)
      ptr->ref();
  }

  vil_image_ref(vil_image_ref const &that) : ptr(that.ptr) {
    if (ptr)
      ptr->ref();
  }

  vil_image_ref &operator==(vil_image_ref const &that) {
    if (ptr != that.ptr) {
      that.ptr->ref();
      ptr->unref();
      
      ptr = that.ptr;
      ptr->ref();
    }
    return *this;
  }

  ~vil_image_ref() {
    if (ptr)
      ptr->unref();
    ptr = 0;
  }

  // "sptr.as_pointer()" is better than "(vil_generic_image*)sptr"
  vil_generic_image *as_pointer() const {
    return ptr;
  }

  // comparison
  bool operator==(vil_image_ref const &that) const {
    return ptr == that.ptr;
  }

  //------------------------------ pointer-like ------------------------------

  // cast to vil_generic_image *
  operator vil_generic_image * () const {
    return ptr; 
  }

  // operator ->
  vil_generic_image *operator -> () const {
    return ptr; 
  }
  
  // dereference
  vil_generic_image &operator * () const {
    return *ptr; 
  }

  // conversion to bool
  operator bool () const {
    return ptr;
  }
  
private:
  vil_generic_image *ptr;
};

#endif
