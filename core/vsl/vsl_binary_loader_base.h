// This is vxl/vsl/vsl_binary_loader_base.h
#ifndef vsl_binary_loader_base_h_
#define vsl_binary_loader_base_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file

//: Base class for vsl_binary_loader objects
//  Available to keep a record of all the loaders, so that they
//  can be deleted to avoid memory leaks.
//  See also vsl_delete_all_loaders().
class vsl_binary_loader_base
{
 public:
  //: Dflt ctor
  vsl_binary_loader_base();

  //: Destructor
  virtual ~vsl_binary_loader_base();

 protected:
  //: Register this, so it can be deleted by vsl_delete_all_loaders();
  void register_this();
};

//: Deletes all the loaders derived from this class
//  Deletes every loader for which register_this() has been called
void vsl_delete_all_loaders();

#endif // vsl_binary_loader_base_h_
