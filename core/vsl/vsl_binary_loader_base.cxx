// This is core/vsl/vsl_binary_loader_base.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "vsl_binary_loader_base.h"
#include <vcl_vector.h>
#include <vcl_vector.txx>
#include <vsl/vsl_indent.h>

// List of all loaders register_this()'ed
// Create on heap so that it can be cleaned up itself
static vcl_vector<vsl_binary_loader_base*> *loader_list_ = 0;


typedef void (*clear_func_ptr) ();
// List of all extra loaders clear funcs registered()'ed
// Create on heap so that it can be cleaned up itself
static vcl_vector<clear_func_ptr> *extra_loader_clear_list_ = 0;


struct vsl_binary_loader_base_auto_clearup
{
  ~vsl_binary_loader_base_auto_clearup()
  {
    vsl_delete_all_loaders();
    // Clear all indent data
    vsl_indent_clear_all_data();
  }
};

static vsl_binary_loader_base_auto_clearup clearup_object;

//=======================================================================

vsl_binary_loader_base::vsl_binary_loader_base()
{
}

//=======================================================================

vsl_binary_loader_base::~vsl_binary_loader_base()
{
}

//=======================================================================
//: Register this, so it can be deleted by vsl_delete_all_loaders();
void vsl_binary_loader_base::register_this()
{
  if (loader_list_==0) loader_list_ = new vcl_vector<vsl_binary_loader_base*>;
  loader_list_->push_back(this);
}


//: Allows other loader scheme to piggy back on the clearing of vsl loaders
// This is useful for getting rid of spurious memory leaks.
void vsl_register_new_loader_clear_func(clear_func_ptr func)
{
  if (extra_loader_clear_list_ ==0)
    extra_loader_clear_list_ = new vcl_vector<clear_func_ptr>;

  extra_loader_clear_list_->push_back(func);
}


//=======================================================================
//: Deletes all the loaders
//  Deletes every loader for which register_this() has been called
void vsl_delete_all_loaders()
{
//  Deletes every vsl loader for which register_this() has been called
  if (loader_list_!=0)
  {
    const unsigned n = loader_list_->size();
    for (unsigned i=0;i<n;++i)
      delete loader_list_->operator[](i);
    loader_list_->clear();

    // Clean up the list itself
    delete loader_list_;
    loader_list_=0;
  }
}
