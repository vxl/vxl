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


#endif // vil_exception_h_

