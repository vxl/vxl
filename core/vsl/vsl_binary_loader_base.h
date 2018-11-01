// This is core/vsl/vsl_binary_loader_base.h
#ifndef vsl_binary_loader_base_h_
#define vsl_binary_loader_base_h_
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
  vsl_binary_loader_base() = default;

  //: Destructor
  virtual ~vsl_binary_loader_base() = default;

 protected:
  //: Register this, so it can be deleted by vsl_delete_all_loaders();
  void register_this();
};


//: Deletes all the loaders derived from this class
//  Deletes every loader for which register_this() has been called
//  This function is automatically called on program exit to ensure
//  memory leak detectors don't throw up false positives.
void vsl_delete_all_loaders();

#endif // vsl_binary_loader_base_h_
