#include <vbl/vbl_smart_ptr.h> // see .h file for authors etc.

// Template definitions for ref() and unref().
// The client can specialize them between including this file and
// calling the instantiation macros, but he should be prepared to 
// implement the protected_ logic in his specialization.
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

//--------------------------------------------------------------------------------

#define VBL_SMART_PTR_INSTANTIATE_inlines(T) \
VCL_INSTANTIATE_INLINE(bool operator== (vbl_smart_ptr<T > const&, vbl_smart_ptr<T > const&)); \
VCL_INSTANTIATE_INLINE(ostream& operator<< (ostream&, vbl_smart_ptr<T > const&));

#undef  VBL_SMART_PTR_INSTANTIATE
#define VBL_SMART_PTR_INSTANTIATE(T) \
template class vbl_smart_ptr<T >;\
VBL_SMART_PTR_INSTANTIATE_inlines(T);
