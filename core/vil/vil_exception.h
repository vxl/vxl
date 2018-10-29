#ifndef vil_exception_h_
#define vil_exception_h_
//:
// \file
// \brief Exceptions thrown by vil, and a mechanism for turning them off.
// \author Ian Scott.

#include <string>
#include <cstdlib>
#include <iostream>
#include <vcl_compiler.h>
#if VCL_HAS_EXCEPTIONS
#include <stdexcept>
#endif
#include <vil/vil_pixel_format.h>


//: Throw an exception indicating a definite problem.
// If exceptions have been disabled, this function
// will abort.
template <class T>
void vil_exception_error(T exception)
{
#if !defined VXL_LEGACY_ERROR_REPORTING && VCL_HAS_EXCEPTIONS
  throw exception;
#else
  std::cerr << "\nERROR: " << exception.what() << std::endl;
  std::abort();
#endif
}

//: Throw an exception indicating a potential problem.
// If exceptions have been disabled, this function
// will return.
template <class T>
void vil_exception_warning(T exception)
{
#if !defined VXL_LEGACY_ERROR_REPORTING && VCL_HAS_EXCEPTIONS
  throw exception;
#else
  std::cerr << "\nWARNING: " << exception.what() << std::endl;
#endif
}

  //: Indicates that a function call failed because the pixel types were incompatible.
class vil_exception_pixel_formats_incompatible
#if VCL_HAS_EXCEPTIONS
  : public std::logic_error
#endif
{
 public:
  enum vil_pixel_format src_type, dest_type;
  std::string operation_name;
  vil_exception_pixel_formats_incompatible(
    enum vil_pixel_format src, enum vil_pixel_format dest, const std::string& operation) :
#if VCL_HAS_EXCEPTIONS
    std::logic_error(operation + ": Pixel formats incompatible."),
#endif
    src_type(src), dest_type(dest), operation_name(operation) {}
#if VCL_HAS_EXCEPTIONS
  ~vil_exception_pixel_formats_incompatible() throw() override = default;
#else
  const char * what() const {return "Pixel formats incompatible.";}
#endif
};


//: Indicates that a function call failed because a pixel format could not be handled.
class vil_exception_unsupported_pixel_format
#if VCL_HAS_EXCEPTIONS
  : public std::logic_error
#endif
{
 public:
  enum vil_pixel_format src_type;
  std::string operation_name;
  vil_exception_unsupported_pixel_format(
    enum vil_pixel_format src, const std::string& operation) :
#if VCL_HAS_EXCEPTIONS
    std::logic_error(operation + ": Unsupported pixel format."),
#endif
    src_type(src), operation_name(operation) {}
#if VCL_HAS_EXCEPTIONS
  ~vil_exception_unsupported_pixel_format() throw() override = default;
#else
  const char * what() const {return "Unsupported Pixel formats.";}
#endif
};


//: Indicates that some reference was made to pixels beyond the bounds of an image.
// In most cases of out-of-bounds access, you will not get this exception. For efficiency
// reasons, vil may not test for this problem, or may if you are lucky trip an assert.
// This function is only used in cases where easy of use, and risk of mistakes are high,
// and inefficiency is very low.
class vil_exception_out_of_bounds
#if VCL_HAS_EXCEPTIONS
  : public std::logic_error
#endif
{
 public:
  std::string operation_name;
  vil_exception_out_of_bounds(
    const std::string& operation) :
#if VCL_HAS_EXCEPTIONS
    std::logic_error(operation + ": Pixel access out-of-bounds."),
#endif
    operation_name(operation) {}
#if VCL_HAS_EXCEPTIONS
  ~vil_exception_out_of_bounds() throw() override = default;
#else
  const char * what() const {return "Pixel access out-of-bounds.";}
#endif
};


//: Indicates that some operation is not supported.
// In most cases you will not get this exception. For efficiency
// reasons, vil may not test for this problem, or may if you are lucky trip an assert.
// This function is only used in cases where easy of use, and risk of mistakes are high,
// and inefficiency is very low.
class vil_exception_unsupported_operation
#if VCL_HAS_EXCEPTIONS
  : public std::logic_error
#endif
{
 public:
  std::string operation_name;
  vil_exception_unsupported_operation(
    const std::string& operation) :
#if VCL_HAS_EXCEPTIONS
    std::logic_error(operation + ": Unsupported operation."),
#endif
    operation_name(operation) {}
#if VCL_HAS_EXCEPTIONS
  ~vil_exception_unsupported_operation() throw() override = default;
#else
  const char * what() const {return "Unsupported operation.";}
#endif
};


//: Indicates that an image load or save operation failed.
class vil_exception_image_io
#if VCL_HAS_EXCEPTIONS
  : public std::runtime_error
#endif
{
 public:
#if !VCL_HAS_EXCEPTIONS
  std::string full_what;
#endif
  std::string function_name, file_type, filename, details;
  vil_exception_image_io(const std::string& function,
                         const std::string& type,
                         const std::string& file_name,
                         const std::string& description = "") :
#if VCL_HAS_EXCEPTIONS
    std::runtime_error
#else
    full_what
#endif
    ("Failed to load " + file_name + " in "
     + function + " using " + type + " loader. " + description),
    function_name(function), file_type(type), filename(file_name), details(description) {}
#if VCL_HAS_EXCEPTIONS
  ~vil_exception_image_io() throw() override = default;
#else
  const char * what() const { return full_what.c_str(); }
#endif
};


//: Indicates unexpected problems image file's data.
// For example, can be thrown when an image file's header suggests an image size or
// file length that is not matched by the actual data present in the file
// Generally this should be thrown, only after checks on the image type
// have been passed by the file format object, and while an
// unrecoverable error is detected inside the image_resource constructor,
// get_view(), etc.
class vil_exception_corrupt_image_file
#if VCL_HAS_EXCEPTIONS
  : public vil_exception_image_io
#endif
{
 public:
  vil_exception_corrupt_image_file(const std::string& function,
                                   const std::string& type,
                                   const std::string& file_name,
                                   const std::string& description = "")
#if VCL_HAS_EXCEPTIONS
  : vil_exception_image_io(function, type, file_name, description)
#endif
  {}
#if VCL_HAS_EXCEPTIONS
  ~vil_exception_corrupt_image_file() throw() override = default;
#else
  const char * what() const {return "Image file is corrupt.";}
#endif
};


//: Indicating an object with an unknown version number
// Indicates that an image file (or subsection thereof) contains data marked as
// a version which iss unknown to the loading code.
class vil_exception_invalid_version
#if VCL_HAS_EXCEPTIONS
  : public vil_exception_image_io
#endif
{
 public:
  vil_exception_invalid_version(const std::string& function,
                                const std::string& type,
                                const std::string& file_name,
                                const std::string& description = "")
#if VCL_HAS_EXCEPTIONS
  : vil_exception_image_io(function, type, file_name, description)
#endif
  {}
#if VCL_HAS_EXCEPTIONS
  ~vil_exception_invalid_version() throw() override = default;
#else
  const char * what() const {return "Unknown version number detected in file";}
#endif
};

#endif // vil_exception_h_
