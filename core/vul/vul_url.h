// This is vxl/vul/vul_url.h
#ifndef vul_url_h_
#define vul_url_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface 
#endif
//:
// \file
// \brief Static class methods to test and open streams via a URL
// \author Ian Scott
// Based on vil_stream_url by fsm
// \verbatim
// Modifications
// 8 Nov 2002 - Peter Vanroose - corrected HTTP client request syntax
// \endverbatim

#include <vcl_istream.h>
#include <vcl_iostream.h> // needed for vcl_ios_in and vcl_ios_openmode
#include <vcl_string.h>

//: Functions to test and open streams via a URL
// Currently supports file and HTTP only.
// HTTP support includes basic authentification, using the normal
// HTTP URL scheme, e.g. \c http://user4:mypassword@webserverthingy.org/file.txt
class vul_url
{
 public:
  //: open a URL
  // If URL is "file://..." open as a file with given mode.
  // If URL is "http://..." open using vul_http_open
  // If URL is "ftp://..." attempt ftp
  // Otherwise assume it is a filename and open with given mode
  static vcl_istream* open(const char* url, vcl_ios_openmode mode=vcl_ios_in );

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
  static vcl_string encode_base64(const vcl_string& in);

  //: Decode a string of chars from base64 format
  static vcl_string decode_base64(const vcl_string& in);
};

#endif // vul_url_h_
