// This is vxl/vsl/vsl_binary_loader_base.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "vsl_binary_loader_base.h"
#include <vcl_vector.h>
#include <vcl_vector.txx>

// List of all loaders register_this()'ed
// Create on heap so that it can be cleaned up itself
static vcl_vector<vsl_binary_loader_base*> *loader_list_ = 0;

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

//=======================================================================
//: Deletes all the loaders
//  Deletes every loader for which register_this() has been called
void vsl_delete_all_loaders()
{
  if (loader_list_==0) return;
  int n = loader_list_->size();
  for (int i=0;i<n;++i)
    delete loader_list_->operator[](i);
  loader_list_->resize(0);

  // Clean up the list itself
  delete loader_list_;
  loader_list_=0;
}
