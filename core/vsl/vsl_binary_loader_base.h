// This is core/vsl/vsl_binary_loader_base.h
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


//: Allows other loader scheme to piggy back on the clearing of vsl loaders
// This is useful for getting rid of spurious memory leaks.
void vsl_register_new_loader_clear_func(void (*func)());

//: Deletes all the loaders derived from this class
//  Deletes every loader for which register_this() has been called
//  This is automatically called on program exit to ensure memory
//  leak detectors don't throw up false positives.  In that spirit,
//  this also calls vsl_indent_clear_all_data() to remove another
//  potential leak.
void vsl_delete_all_loaders();

#endif // vsl_binary_loader_base_h_
