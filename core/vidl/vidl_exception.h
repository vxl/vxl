// This is core/vidl/vidl_exception.h
#ifndef vidl_exception_h_
#define vidl_exception_h_
//=========================================================================
//:
// \file
// \brief  Exceptions thrown by vidl.
// \author Miguel A. Figueroa-Villanueva (miguelf at ieee dot org)
//
// This file contains a replica of Ian Scott's exception mechanism as
// coded in vil_exception.h and mbl_exception.h. However, it encapsulates
// the exceptions thrown by the vidl library.
//
// Note that vidl_exception_error, vidl_exception_warning, and possibly
// a vxl/vbl_exception should really be part of vbl. So that any level-2
// library can use them.
//
// \verbatim
//  Modifications
//   01/24/2006 - File created. (miguelfv)
//   03/07/2006 - File imported to vxl repository with some modifications
//                and extensions to Paul's code. (miguelfv)
// \endverbatim
//
//=========================================================================

#include <iostream>
#include <string>
#include <utility>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//-------------------------------------------------------------------------
//: Throw an exception indicating a definite problem.
// If exceptions have been disabled, this function will abort.
//-------------------------------------------------------------------------
template <class T> void vidl_exception_error(T exception)
{
  std::cerr << "\nERROR: " << exception.what() << std::endl;

#if !defined VIDL_EXCEPTIONS_DISABLE
  throw exception;
#else
  std::abort();
#endif
}

//-------------------------------------------------------------------------
//: Throw an exception indicating a potential problem.
// If exceptions have been disabled, this function will return.
//-------------------------------------------------------------------------
template <class T> void vidl_exception_warning(T exception)
{
  std::cerr << "\nWARNING: " << exception.what() << std::endl;

#if !defined VIDL_EXCEPTIONS_DISABLE
  throw exception;
#endif
}

//-------------------------------------------------------------------------
// The list of exceptions.
//-------------------------------------------------------------------------
//: Base class of all vidl exceptions.
class vidl_exception
{
 public:
  explicit vidl_exception(std::string  msg) : msg_(std::move(msg)) {}
  virtual ~vidl_exception() = default;

  virtual const std::string& what() const { return msg_; }

 private:
  std::string msg_;
};

//: Base class for all the DShow related vidl exceptions.
struct vidl_dshow_exception : public vidl_exception
{
  explicit vidl_dshow_exception(const std::string& msg)
    : vidl_exception("DShow: " + msg) {}
};

#endif // vidl_exception_h_
