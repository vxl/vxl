// This is core/vul/vul_url.h
#ifndef vul_url_h_
#define vul_url_h_
//:
// \file
// \brief Static class methods to test and open streams via a URL
// \author Ian Scott
// Based on vil_stream_url by fsm
// \verbatim
// Modifications
// 8 Nov 2002 - Peter Vanroose - corrected HTTP client request syntax
// \endverbatim

#include <istream>
#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Functions to test and open streams via a URL
// Currently supports file and HTTP only.
// HTTP support includes basic authentication, using the normal
// HTTP URL scheme, e.g. \c http://user4:mypassword@webserverthingy.org/file.txt
class vul_url
{
 public:
  //: open a URL
  // If URL is "file://..." open as a file with given mode.
  // If URL is "http://..." open using vul_http_open
  // If URL is "ftp://..." attempt ftp
  // Otherwise assume it is a filename and open with given mode
  static std::istream* open(const char* url, std::ios::openmode mode=std::ios::in );

  //: Does that URL exist
  // If the URL does not begin with a recognised scheme identifier, the function will
  // treat the parameter as a local filename
  static bool exists(const char* url);

  //: Is that a URL
  // as opposed to an ordinary filename.
  static bool is_url(const char* url);

  //: Is that a file
  // i.e. is it a downloadable URL, or a file on disk that isn't a directory.
  static bool is_file(const char* url);

  //: Encode a string of chars into base64 format
  static std::string encode_base64(const std::string& in);

  //: Decode a string of chars from base64 format
  static std::string decode_base64(const std::string& in);
};

#endif // vul_url_h_
