// This is vxl/vsl/vsl_clipon_binary_loader.txx
#ifndef vsl_clipon_binary_loader_txx_
#define vsl_clipon_binary_loader_txx_
//:
// \file

#include "vsl_clipon_binary_loader.h"
#include <vcl_cstdlib.h> // vcl_abort()
#include <vcl_vector.txx>

template<class BaseClass, class BaseClassIO>
vsl_clipon_binary_loader<BaseClass,BaseClassIO>& vsl_clipon_binary_loader<BaseClass,BaseClassIO>::instance()
{
  if (instance_ == 0)
  {
    instance_ = new vsl_clipon_binary_loader<BaseClass,BaseClassIO>;

    // Register for deletion by vsl_delete_all_loaders()
    instance_->register_this();
  }
  return *instance_;
}

//: Add example object to list of those that can be loaded
template<class BaseClass, class BaseClassIO>
void vsl_clipon_binary_loader<BaseClass,BaseClassIO>::add( const BaseClassIO& b)
{
  object_io_.push_back(b.clone());
}

//: Return index associated with given object name
template<class BaseClass, class BaseClassIO>
int vsl_clipon_binary_loader<BaseClass,BaseClassIO>::index_for_name(const vcl_string& name) const
{
  unsigned int i=0;
  while ((i<object_io_.size()) && !(object_io_[i]->target_classname()==name))
    ++i;

  if (i>=object_io_.size())
  {
    vcl_cerr << "vsl_clipon_binary_loader<BaseClass>::index_for_name: ";
    vcl_cerr << "class name <" << name << "> not in list of loaders"<<vcl_endl;
    vcl_cerr << object_io_.size()<<" valid loaders available for "<<vcl_endl;
    for (unsigned int j=0; j<object_io_.size(); ++j)
      vcl_cerr << object_io_[j]->target_classname() << vcl_endl;
    vcl_abort();
  }

  return i;
}

//: Return IO object that can deal with given class
template<class BaseClass, class BaseClassIO>
const BaseClassIO& vsl_clipon_binary_loader<BaseClass,BaseClassIO>::io_for_class(const BaseClass& b) const
{
  unsigned int i;
  for (i=0; (i<object_io_.size()) && !(object_io_[i]->is_io_for(b)); i++);

  if (i>=object_io_.size())
  {
    vcl_cerr << "vsl_clipon_binary_loader<BaseClass>::io_for_class: ";
    vcl_cerr << "Unable to determine suitable loader."<<vcl_endl;
    vcl_cerr << object_io_.size()<<" valid loaders available for "<<vcl_endl;
    for (unsigned int j=0; j<object_io_.size(); ++j)
      vcl_cerr << object_io_[j]->target_classname() << vcl_endl;
    vcl_abort();
  }

  return *object_io_[i];
}

//: Return IO object for given named class
template<class BaseClass, class BaseClassIO>
const BaseClassIO& vsl_clipon_binary_loader<BaseClass,BaseClassIO>::object_io(const vcl_string& name) const
{
  return *object_io_[index_for_name(name)];
}


template<class BaseClass, class BaseClassIO>
void vsl_clipon_binary_loader<BaseClass,BaseClassIO>::make_empty()
{
  for (unsigned int i=0; i<object_io_.size(); ++i)
  delete object_io_[i];
  object_io_.resize(0);
}

template<class BaseClass, class BaseClassIO>
vsl_clipon_binary_loader<BaseClass,BaseClassIO>::~vsl_clipon_binary_loader()
{
  make_empty();
}

// IO for  pointers to BaseClass:
template<class BaseClass, class BaseClassIO>
void vsl_clipon_binary_loader<BaseClass,BaseClassIO>::read_object( vsl_b_istream& is, BaseClass*& b)
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

  const BaseClassIO& io = object_io(name);
  b = io.new_object();
  io.b_read_by_base(is,*b);
}

// IO for  pointers to BaseClass:
template<class BaseClass, class BaseClassIO>
void vsl_clipon_binary_loader<BaseClass,BaseClassIO>::write_object( vsl_b_ostream& os, const BaseClass* b)
{
  if (b==0)
  {
    vsl_b_write(os,vcl_string("VSL_NULL_PTR"));
    return;
  }

  const BaseClassIO& io = io_for_class(*b);
  vsl_b_write(os,io.target_classname());
  io.b_write_by_base(os,*b);
}

// IO for  pointers to BaseClass:
template<class BaseClass, class BaseClassIO>
void vsl_clipon_binary_loader<BaseClass,BaseClassIO>::print_object_summary( vcl_ostream& os, const BaseClass* b)
{
  if (b==0)
  {
    os<<"No object defined.";
    return;
  }

  const BaseClassIO& io = io_for_class(*b);
  io.print_summary_by_base(os,*b);
}

template <class B, class IO>
vsl_clipon_binary_loader<B, IO>* vsl_clipon_binary_loader<B, IO>::instance_ = 0;

#define VSL_CLIPON_BINARY_LOADER_INSTANTIATE(B,IO) \
template class vsl_clipon_binary_loader<B, IO >; \
VCL_VECTOR_INSTANTIATE(IO*)

#endif // vsl_clipon_binary_loader_txx_
