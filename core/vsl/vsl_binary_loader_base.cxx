// This is core/vsl/vsl_binary_loader_base.cxx
//:
// \file

#include <mutex>
#include <vector>
#include "vsl_binary_loader_base.h"
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "vsl/vsl_indent.h"

using clear_func_ptr = void (*)();

// Replaced the original raw "static T * = nullptr" + lazy
// "if (p == nullptr) p = new T" pattern with a holder struct that
// owns its mutex and registry vectors as members. Bundling the mutex
// and the data into one Meyers singleton guarantees they share
// lifetime: the mutex cannot be destroyed before the destructor that
// locks it. (Two separate function-local statics would have ordered
// their destruction by reverse-of-construction, which on this code
// path produced a "mutex lock failed: Invalid argument" abort at
// program exit when the mutex was constructed lazily after the
// registry.)
struct vsl_binary_loader_base_auto_clearup
{
  std::mutex mtx;
  std::vector<vsl_binary_loader_base *> loaders;
  std::vector<clear_func_ptr> clear_funcs;

  ~vsl_binary_loader_base_auto_clearup()
  {
    vsl_delete_all_loaders();
    // Clear all indent data
    vsl_indent_clear_all_data();
  }
};

static vsl_binary_loader_base_auto_clearup &
vsl_loader_registry()
{
  static vsl_binary_loader_base_auto_clearup r;
  return r;
}

// Force initialization at static-construction time so the program-exit
// destructor still runs even if no loader is ever explicitly
// registered.
static auto & vsl_loader_registry_ref = vsl_loader_registry();

//=======================================================================
//: Register this, so it can be deleted by vsl_delete_all_loaders();
void
vsl_binary_loader_base::register_this()
{
  auto & registry = vsl_loader_registry();
  std::lock_guard<std::mutex> guard(registry.mtx);
  vsl_loader_registry().loaders.push_back(this);
}


//: Allows other loader scheme to piggy back on the clearing of vsl loaders
// This is useful for getting rid of spurious memory leaks.
void
vsl_register_new_loader_clear_func(clear_func_ptr func)
{
  auto & registry = vsl_loader_registry();
  std::lock_guard<std::mutex> guard(registry.mtx);
  vsl_loader_registry().clear_funcs.push_back(func);
}


//=======================================================================
//: Deletes all the loaders
//  Deletes every loader for which register_this() has been called.
//  Note: clear_funcs registered via vsl_register_new_loader_clear_func
//  are NOT invoked here; that matches the historical (pre-thread-safe)
//  behavior of this function. Whether the clear_funcs path was meant
//  to fire at program exit is a separate question that should be
//  addressed independently of this thread-safety fix.
void
vsl_delete_all_loaders()
{
  auto & registry = vsl_loader_registry();
  std::lock_guard<std::mutex> guard(registry.mtx);

  for (auto * loader : registry.loaders)
    delete loader;
  registry.loaders.clear();
}
