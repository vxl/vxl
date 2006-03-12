// This is brl/bbas/vidl2/vidl2_exception.h
#ifndef vidl2_exception_h_
#define vidl2_exception_h_
//=========================================================================
//:
// \file
// \brief  Exceptions thrown by vidl2.
// \author Miguel A. Figueroa-Villanueva (miguelf at ieee dot org)
//
// This file contains a replica of Ian Scott's exception mechanism as
// coded in vil_exception.h and mbl_exception.h. However, it encapsulates
// the exceptions thrown by the vidl2 library.
//
// Note that vidl2_exception_error, vidl2_exception_warning, and possibly
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

#include <vcl_iostream.h>
#include <vcl_string.h>

//-------------------------------------------------------------------------
//: Throw an exception indicating a definite problem.
// If exceptions have been disabled, this function will abort.
//-------------------------------------------------------------------------
template <class T> void vidl2_exception_error(T exception)
{
  vcl_cerr << "\nERROR: " << exception.what() << vcl_endl;

#if !defined VIDL2_EXCEPTIONS_DISABLE && VCL_HAS_EXCEPTIONS
  throw exception;
#else
  vcl_abort();
#endif
}

//-------------------------------------------------------------------------
//: Throw an exception indicating a potential problem.
// If exceptions have been disabled, this function will return.
//-------------------------------------------------------------------------
template <class T> void vidl2_exception_warning(T exception)
{
  vcl_cerr << "\nWARNING: " << exception.what() << vcl_endl;

#if !defined VIDL2_EXCEPTIONS_DISABLE && VCL_HAS_EXCEPTIONS
  throw exception;
#endif
}

//-------------------------------------------------------------------------
// The list of exceptions.
//-------------------------------------------------------------------------
//: Base class of all vidl2 exceptions.
class vidl2_exception
{
 public:
  explicit vidl2_exception(const vcl_string& msg) : msg_(msg) {}
  virtual ~vidl2_exception() {}

  virtual const vcl_string& what() const { return msg_; }

 private:
  vcl_string msg_;
};

//: Base class for all the DShow related vidl2 exceptions.
struct vidl2_dshow_exception : public vidl2_exception
{
  explicit vidl2_dshow_exception(const vcl_string& msg)
    : vidl2_exception("DShow: " + msg) {}
};

#endif // vidl2_exception_h_
