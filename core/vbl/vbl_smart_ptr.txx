//-----------------------------------------------------------------------------
//
// .NAME vbl_smart_ptr - A templated smart pointer class.
// .LIBRARY vbl
// .HEADER vbl 
// .INCLUDE vbl/vbl_smart_ptr.h
// .FILE vbl_smart_ptr.h
// .EXAMPLE vbl_smart_ptr_example.C
//
// .SECTION Description
//   This class requires that the class being templated over has 
// a unref and ref method, and the stream operator<< function.  Currently,
// it has only been tested with RefCntTimeStampMixin.
// 
// WARNING : Please do not make an automatic cast to T*.
//           This is perceived as dangerous.
//
// .SECTION See also
//   RefCntTimeStampMixin
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
// Builds off the RefCntTimeStampMixin to give smart pointers
//

#include <vbl/vbl_smart_ptr.h>

template <class T>
void vbl_smart_ptr<T>::ref()
  {
    if (ptr_) 						
      {							
        ptr_->ref(); 					
        protected_ = true;					
      }		
  }

template <class T>
void vbl_smart_ptr<T>::unref()
  { 							
    if (ptr_ && protected_) 					
      {							
	ptr_->unref();
	protected_ = false; /* sure we do not do it twice */	
      }							
  }		 

#undef  VBL_SMART_PTR_INSTANTIATE
#define VBL_SMART_PTR_INSTANTIATE(T)                              \
  template class vbl_smart_ptr<T>;                                \
  VCL_INSTANTIATE_INLINE(bool operator== (vbl_smart_ptr<T> const&, vbl_smart_ptr<T> const&)); \
  VCL_INSTANTIATE_INLINE(ostream& operator<< (ostream&, vbl_smart_ptr<T> const&)); \


