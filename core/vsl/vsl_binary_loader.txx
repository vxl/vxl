// This is vxl/vsl/vsl_binary_loader.txx
#ifndef vsl_binary_loader_txx_
#define vsl_binary_loader_txx_
//:
// \file

#include "vsl_binary_loader.h"
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
    for (unsigned int i=0; i<object_.size(); ++i)
      delete object_[i];
    object_.resize(0);
}

template<class BaseClass>
vsl_binary_loader<BaseClass>::~vsl_binary_loader()
{
    make_empty();
    instance_=0;
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

  vcl_string name;
  vsl_b_read(is,name);

  if (name=="VSL_NULL_PTR")
  {
    // Zero pointer
    b=0;
    return;
  }

  unsigned int i;
  for (i=0; (i<object_.size()) && !(object_[i]->is_a()==name); i++);

  if (i<object_.size())
  {
    b = object_[i]->clone(); // If you get a compile error here you are probably trying to load into a non-base class pointer.
    b->b_read(is);
  }
  else
  {
    vcl_cerr << "\n I/O ERROR: vsl_binary_loader<BaseClass>::load_object: ";
    vcl_cerr << "class name <" << name << "> not in list of loaders"<<vcl_endl;
    vcl_cerr << object_.size()<<" valid loaders: "<<vcl_endl;
    for (unsigned int j=0; j<object_.size(); ++j)
      vcl_cerr << object_[j]->is_a() << vcl_endl;
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

#ifdef VCL_SGI_CC
// This specialisation is needed since otherwise the more general template
// void vsl_b_write(vsl_b_ostream& bfs, const BaseClass* b)
// would be used for const char*, which is not correct.
// This specialisation must of course precede the more general definition.
//
// Note that this must be declared "inline", since this function is already
// implemented (and exported) in vsl_binary_io.cxx; exporting it from here
// as well would give linker errors (multiple definition of the same symbol).
//
// An alternative solution is a single inline definition in vsl_binary_io.h
// This definition must be implemented identically to the one in
// vsl_binary_io.h, otherwise there may be a io incompatibility.

inline void vsl_b_write(vsl_b_ostream& bfs, const char* b)
{
  int i = -1;
  do {
     i++;
     vsl_b_write(bfs,b[i]);
  } while ( b[i] != 0 );
}
#endif

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
    vsl_b_write(bfs, vcl_string("VSL_NULL_PTR"));
}

template <class BaseClass>
vsl_binary_loader<BaseClass>* vsl_binary_loader<BaseClass>::instance_ = 0;


#undef VSL_BINARY_LOADER_INSTANTIATE
#define VSL_BINARY_LOADER_WITH_SPECIALIZATION_INSTANTIATE(T) \
template class vsl_binary_loader<T >
#define VSL_BINARY_LOADER_INSTANTIATE(T) \
VSL_BINARY_LOADER_WITH_SPECIALIZATION_INSTANTIATE(T); \
VCL_INSTANTIATE_INLINE(void vsl_b_read( vsl_b_istream& bfs, T*& b)); \
template void vsl_b_write(vsl_b_ostream& bfs, const T* b)


#endif // vsl_binary_loader_txx_
