// This is core/vsl/vsl_binary_loader.hxx
#ifndef vsl_binary_loader_hxx_
#define vsl_binary_loader_hxx_
//:
// \file

#include <iostream>
#include <vector>
#include "vsl_binary_loader.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template<class BaseClass>
vsl_binary_loader<BaseClass>& vsl_binary_loader<BaseClass>::instance()
{
  if (instance_ == nullptr)
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
  for (unsigned int i=0; i<object_.size(); ++i)
    delete object_[i];
  object_.resize(0);
}

template<class BaseClass>
vsl_binary_loader<BaseClass>::~vsl_binary_loader()
{
  make_empty();
  instance_=nullptr;
}

// IO for  pointers to BaseClass:
template<class BaseClass>
void vsl_binary_loader<BaseClass>::load_object( vsl_b_istream& is, BaseClass*& b)
{
  if (!is) return;

  // HELP ON RUN-TIME ERROR HERE
  // If you get a run-time error here it is most-likely because you called
  // vsl_b_read with an uninitialised null base_class pointer. The base class
  // pointer should either point to a real object, or be set to 0 - IMS.
  delete b;   // Delete old object pointed to by b

  std::string name;
  vsl_b_read(is,name);

  if (name=="VSL_NULL_PTR")
  {
    // Zero pointer
    b=nullptr;
    return;
  }

  unsigned int i = 0;
  while (i<object_.size() && !(object_[i]->is_a()==name)) ++i;

  if (i<object_.size())
  {
    b = object_[i]->clone(); // If you get a compile error here you are probably trying to load into a non-base class pointer.
    b->b_read(is);
  }
  else
  {
    std::cerr << "\n I/O ERROR: " << is_a() << "::load_object: "
             << "class name <" << name << "> not in list of loaders\n"
             << object_.size()<<" valid loaders:\n";
    for (unsigned int j=0; j<object_.size(); ++j)
      std::cerr << object_[j]->is_a() << std::endl;
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

// For pointer to baseclass types, but *not* for char* !
template<class BaseClass>
void vsl_b_write(vsl_b_ostream& bfs, const BaseClass* b)
{
  if (b)
  {
    vsl_b_write(bfs, b->is_a());
    b->b_write(bfs);
  }
  else
    vsl_b_write(bfs, std::string("VSL_NULL_PTR"));
}

template <class BaseClass>
vsl_binary_loader<BaseClass>* vsl_binary_loader<BaseClass>::instance_ = nullptr;


#undef VSL_BINARY_LOADER_INSTANTIATE
#define VSL_BINARY_LOADER_WITH_SPECIALIZATION_INSTANTIATE(T) \
template <> std::string vsl_binary_loader<T >::is_a() const \
{  return std::string("vsl_binary_loader<" #T ">"); }\
template class vsl_binary_loader<T >
#define VSL_BINARY_LOADER_INSTANTIATE(T) \
VSL_BINARY_LOADER_WITH_SPECIALIZATION_INSTANTIATE(T); \
/*template void vsl_b_read( vsl_b_istream& bfs, (T)*& b) ; */ \
template void vsl_b_write(vsl_b_ostream& bfs, const T* b)

#endif // vsl_binary_loader_hxx_
