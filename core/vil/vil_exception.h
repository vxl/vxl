#ifndef vil_exception_h_
#define vil_exception_h_
#define VXL_LEGACY_ERROR_REPORTING // REQUIRED FOR PASSING TESTS 2018-11-02
//:
// \file
// \brief Exceptions thrown by vil, and a mechanism for turning them off.
// \author Ian Scott.

#include <string>
#include <cstdlib>
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <stdexcept>
#include <vil/vil_pixel_format.h>


//: Throw an exception indicating a definite problem.
// If exceptions have been disabled, this function
// will abort.
template <class T>
void vil_exception_error(T exception)
{
#if !defined VXL_LEGACY_ERROR_REPORTING
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
#if !defined VXL_LEGACY_ERROR_REPORTING
  throw exception;
#else
  std::cerr << "\nWARNING: " << exception.what() << std::endl;
#endif
}

  //: Indicates that a function call failed because the pixel types were incompatible.
class vil_exception_pixel_formats_incompatible
  : public std::logic_error
{
 public:
  enum vil_pixel_format src_type, dest_type;
  std::string operation_name;
  vil_exception_pixel_formats_incompatible(
    enum vil_pixel_format src, enum vil_pixel_format dest, const std::string& operation) :
    std::logic_error(operation + ": Pixel formats incompatible."),
    src_type(src), dest_type(dest), operation_name(operation) {}
  ~vil_exception_pixel_formats_incompatible() throw() override = default;
};


//: Indicates that a function call failed because a pixel format could not be handled.
class vil_exception_unsupported_pixel_format
  : public std::logic_error
{
 public:
  enum vil_pixel_format src_type;
  std::string operation_name;
  vil_exception_unsupported_pixel_format(
    enum vil_pixel_format src, const std::string& operation) :
    std::logic_error(operation + ": Unsupported pixel format."),
    src_type(src), operation_name(operation) {}
  ~vil_exception_unsupported_pixel_format() throw() override = default;
};


//: Indicates that some reference was made to pixels beyond the bounds of an image.
// In most cases of out-of-bounds access, you will not get this exception. For efficiency
// reasons, vil may not test for this problem, or may if you are lucky trip an assert.
// This function is only used in cases where easy of use, and risk of mistakes are high,
// and inefficiency is very low.
class vil_exception_out_of_bounds
  : public std::logic_error
{
 public:
  std::string operation_name;
  vil_exception_out_of_bounds(
    const std::string& operation) :
    std::logic_error(operation + ": Pixel access out-of-bounds."),
    operation_name(operation) {}
  ~vil_exception_out_of_bounds() throw() override = default;
};


//: Indicates that some operation is not supported.
// In most cases you will not get this exception. For efficiency
// reasons, vil may not test for this problem, or may if you are lucky trip an assert.
// This function is only used in cases where easy of use, and risk of mistakes are high,
// and inefficiency is very low.
class vil_exception_unsupported_operation
  : public std::logic_error
{
 public:
  std::string operation_name;
  vil_exception_unsupported_operation(
    const std::string& operation) :
    std::logic_error(operation + ": Unsupported operation."),
    operation_name(operation) {}
  ~vil_exception_unsupported_operation() throw() override = default;
};


//: Indicates that an image load or save operation failed.
class vil_exception_image_io
  : public std::runtime_error
{
 public:
  std::string function_name, file_type, filename, details;
  vil_exception_image_io(const std::string& function,
                         const std::string& type,
                         const std::string& file_name,
                         const std::string& description = "") :
    std::runtime_error
    ("Failed to load " + file_name + " in "
     + function + " using " + type + " loader. " + description),
    function_name(function), file_type(type), filename(file_name), details(description) {}
  ~vil_exception_image_io() throw() override = default;
};


//: Indicates unexpected problems image file's data.
// For example, can be thrown when an image file's header suggests an image size or
// file length that is not matched by the actual data present in the file
// Generally this should be thrown, only after checks on the image type
// have been passed by the file format object, and while an
// unrecoverable error is detected inside the image_resource constructor,
// get_view(), etc.
class vil_exception_corrupt_image_file
  : public vil_exception_image_io
{
 public:
  vil_exception_corrupt_image_file(const std::string& function,
                                   const std::string& type,
                                   const std::string& file_name,
                                   const std::string& description = "")
  : vil_exception_image_io(function, type, file_name, description)
  {}
  ~vil_exception_corrupt_image_file() throw() override = default;
};


//: Indicating an object with an unknown version number
// Indicates that an image file (or subsection thereof) contains data marked as
// a version which iss unknown to the loading code.
class vil_exception_invalid_version
  : public vil_exception_image_io
{
 public:
  vil_exception_invalid_version(const std::string& function,
                                const std::string& type,
                                const std::string& file_name,
                                const std::string& description = "")
  : vil_exception_image_io(function, type, file_name, description)
  {}
  ~vil_exception_invalid_version() throw() override = default;
};

#endif // vil_exception_h_
