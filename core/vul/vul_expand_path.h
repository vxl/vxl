// This is core/vul/vul_expand_path.h
#ifndef vul_expand_path_h_
#define vul_expand_path_h_
//:
// \file
// \brief Contains two functions to compute expanded form of a given path.
//
// Given a path in the file system, compute an expanded form.
// An expanded path is one which does not
//  - involve any symbolic links
//  - involve any . or ..
//  - begin with a ~
//  - contain any trailing or repeated /
//
// The result of each query is cached so that a subsequent
// query on the same path will be much faster and will not
// involve any system calls.
//
// \author fsm
//
// \verbatim
// Modifications
// PDA (Manchester) 21/03/2001: Tidied up the documentation
// Peter Vanroose   27/05/2001: Corrected the documentation
// \endverbatim

#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vxl_config.h>

//: Expand given path.
std::string vul_expand_path/*cached*/(std::string path);

//: Expand given path.
std::string vul_expand_path_uncached (std::string path);

#if defined(_WIN32) && VXL_USE_WIN_WCHAR_T

//: Expand given path.
std::wstring vul_expand_path/*cached*/(std::wstring path);

//: Expand given path.
std::wstring vul_expand_path_uncached (std::wstring path);

#endif

#endif // vul_expand_path_h_
