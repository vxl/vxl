#ifndef vsl_binary_loader_txx_
#define vsl_binary_loader_txx_

#include <vsl/vsl_binary_loader.h>
#include <vcl_vector.txx>

template<class BaseClass>
vsl_binary_loader<BaseClass>& vsl_binary_loader<BaseClass>::instance()
{
  if (instance_ == 0)
  {
    instance_ = new vsl_binary_loader<BaseClass>;
    
    // Register for deletion by vsl_delete_all_loaders()
    instance_->register_this();
  }
  return *instance_;
}

template<class BaseClass>
void vsl_binary_loader<BaseClass>::make_empty()
{
    for (int i=0; i<object_.size(); ++i)
    delete object_[i];
    object_.resize(0);
}

template<class BaseClass>
vsl_binary_loader<BaseClass>::~vsl_binary_loader()
{
    make_empty();
}

// IO for  pointers to BaseClass:
template<class BaseClass>
void vsl_binary_loader<BaseClass>::load_object( vsl_b_istream& is, BaseClass*& b)
{
  // Delete old object pointed to by b
  delete b;

  vcl_string name;
  vsl_b_read(is,name);

  if (name=="VSL_NULL_PTR")
  {
    // Zero pointer
    b=0;
    return;
    }

  int i;
  for (i=0; (i<object_.size()) && !(object_[i]->is_a()==name); i++);

  if (i<object_.size())
  {
      b = object_[i]->clone(); // If you get a compile error here you are probably trying to load into a non-base class pointer.
      b->b_read(is);
  }
  else
  {
    vcl_cerr << "EEROR: vsl_binary_loader<BaseClass>::load_object: ";
    vcl_cerr << "class name <" << name << "> not in list of loaders"<<vcl_endl;
    vcl_cerr << object_.size()<<" valid loaders: "<<vcl_endl;
    for (int j=0; j<object_.size(); ++j)
      vcl_cerr << object_[j]->is_a() << vcl_endl;
    vcl_abort();
  }
}

template <class T>
vsl_binary_loader<T>* vsl_binary_loader<T>::instance_ = 0;

#define VSL_BINARY_LOADER_INSTANTIATE(T) \
template class vsl_binary_loader<T >; \
template void vsl_b_read( vsl_b_istream& bfs, T*& b); \
VCL_VECTOR_INSTANTIATE(T*)



#if 0 // is this really needed? why?
#if VCL_CAN_DO_STATIC_TEMPLATE_MEMBER
#ifdef __GNUC__
#define VNL_BINARY_LOADER_INSTANTIATE(T) \
vsl_binary_loader<T >* vsl_binary_loader<T >::instance_ = 0; \
template class vsl_binary_loader<T >; \
template void vsl_b_read( vsl_b_istream& bfs, T*& b); \
/* Create space for singleton pointer */ \
VCL_VECTOR_INSTANTIATE(T*)
#else
#define VNL_BINARY_LOADER_INSTANTIATE(T) \
template class vsl_binary_loader<T >; \
template void vsl_b_read( vsl_b_istream& bfs, T*& b); \
/* Create space for singleton pointer */ \
VCL_VECTOR_INSTANTIATE(T*)

template <class T> vsl_binary_loader<T >* vsl_binary_loader<T >::instance_ = 0;

#endif
#else
#define VNL_BINARY_LOADER_INSTANTIATE(T) \
template class vsl_binary_loader<T >; \
template void vsl_b_read( vsl_b_istream& bfs, T*& b); \
/* Create space for singleton pointer */ \
VCL_VECTOR_INSTANTIATE(T*)
#endif
#endif

#endif
