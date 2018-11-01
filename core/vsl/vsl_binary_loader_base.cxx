// This is core/vsl/vsl_binary_loader_base.cxx
//:
// \file

#include <vector>
#include "vsl_binary_loader_base.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_indent.h>

// List of all loaders register_this()'ed
// Create on heap so that it can be cleaned up itself
static std::vector<vsl_binary_loader_base*> *loader_list_ = nullptr;


typedef void (*clear_func_ptr) ();
// List of all extra loaders clear funcs registered()'ed
// Create on heap so that it can be cleaned up itself
static std::vector<clear_func_ptr> *extra_loader_clear_list_ = nullptr;


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
//: Register this, so it can be deleted by vsl_delete_all_loaders();
void vsl_binary_loader_base::register_this()
{
  if (loader_list_==nullptr) loader_list_ = new std::vector<vsl_binary_loader_base*>;
  loader_list_->push_back(this);
}


//: Allows other loader scheme to piggy back on the clearing of vsl loaders
// This is useful for getting rid of spurious memory leaks.
void vsl_register_new_loader_clear_func(clear_func_ptr func)
{
  if (extra_loader_clear_list_ ==nullptr)
    extra_loader_clear_list_ = new std::vector<clear_func_ptr>;

  extra_loader_clear_list_->push_back(func);
}


//=======================================================================
//: Deletes all the loaders
//  Deletes every loader for which register_this() has been called
void vsl_delete_all_loaders()
{
//  Deletes every vsl loader for which register_this() has been called
  if (loader_list_!=nullptr)
  {
    const auto n = (unsigned int)(loader_list_->size());
    for (unsigned i=0;i<n;++i)
      delete loader_list_->operator[](i);
    loader_list_->clear();

    // Clean up the list itself
    delete loader_list_;
    loader_list_=nullptr;
  }
}
