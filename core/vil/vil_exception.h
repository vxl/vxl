#ifndef vil_exception_h_
#define vil_exception_h_
//:
// \file
// \brief Exceptions thrown by vil, and a mechanism for turning them off.
// \author Ian Scott.

#include <vcl_string.h>
#include <vcl_cstdlib.h>
#include <vcl_iostream.h>
#if VCL_HAS_EXCEPTIONS
# include <vcl_stdexcept.h>
#endif
#include <vil/vil_pixel_format.h>


//: Throw an exception indicating a definite problem.
// If exceptions have been disabled, this function
// will abort.
template <class T>
void vil_exception_error(T exception)
{
  vcl_cerr << "\nERROR: " << exception.what() << vcl_endl;
#if !defined VIL_EXCEPTIONS_DISABLE  && VCL_HAS_EXCEPTIONS
  throw exception;
#else
  vcl_abort();
#endif
}

//: Throw an exception indicating a potential problem.
// If exceptions have been disabled, this function
// will return.
template <class T>
void vil_exception_warning(T exception)
{
  vcl_cerr << "\nWARNING: " << exception.what() << vcl_endl;
#if !defined VIL_EXCEPTIONS_DISABLE  && VCL_HAS_EXCEPTIONS
  throw exception;
#endif
}



#if !VCL_HAS_EXCEPTIONS

  //: Indicates that an image assignment failed because the pixel types were incompatible.
  class vil_exception_assignment_pixel_formats_incompatible
  {
   public:
    enum vil_pixel_format src_type, dest_type;
    vil_exception_assignment_pixel_formats_incompatible(
      enum vil_pixel_format src, enum vil_pixel_format dest)
      : src_type(src), dest_type(dest) {}
    const char * what() const {return "Pixel formats incompatible during assignment operation.";}
  };

#else

  //: Indicates that an image assignment failed because the pixel types were incompatible.
  class vil_exception_assignment_pixel_formats_incompatible : public vcl_logic_error
  {
   public:
    enum vil_pixel_format src_type, dest_type;
    vil_exception_assignment_pixel_formats_incompatible(
      enum vil_pixel_format src, enum vil_pixel_format dest)
      : vcl_logic_error(vcl_string("Pixel formats incompatible during assignment operation.")),
      src_type(src), dest_type(dest) {}
    virtual ~vil_exception_assignment_pixel_formats_incompatible() throw() {}
  };

#endif

#if !VCL_HAS_EXCEPTIONS

  //: Indicates that an image load or save operation failed.
  // Generally this should be thrown, only after checks on the image type
  // have been passed by the file format object, and while an
  // unrecoverable error is detected inside the image_resource constructor,
  // or similar.
  class vil_exception_image_io
  {
   public:
    vcl_string function_name, file_type, filename, full_what;
    vil_exception_image_io(const vcl_string& function,
      const vcl_string& type, const vcl_string& name):
      function_name(function), file_type(type), filename(name),
      full_what("Unrecoverable failure in " + function + " while loading "
        + name + " using " + type + " loader.") {}
    const char * what() const {return full_what.c_str();}
  };

#else
  //: Indicates that an image load or save operation failed.
  // Generally this should be thrown, only after checks on the image type
  // have been passed by the file format object, and while an
  // unrecoverable error is detected inside the image_resource constructor,
  // or similar.
  class vil_exception_image_io : public vcl_runtime_error
  {
   public:
    vcl_string function_name, file_type, filename;
    vil_exception_image_io(const vcl_string& function,
      const vcl_string& type, const vcl_string& name):
      function_name(function), file_type(type), filename(name),
      vcl_runtime_error("Unrecoverable failure in " + function + 
        " while loading " + name + " using " + type + " loader.") {}
    virtual ~vil_exception_image_io() throw() {}
  };

#endif


#endif // vil_exception_h_

