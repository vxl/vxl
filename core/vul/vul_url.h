#ifndef vul_url_h_
#define vul_url_h_
#ifdef __GNUC__
#pragma interface 
#endif

#include <vcl_istream.h>
#include <vcl_string.h>

//: Functions to test and open streams via a URL
class vul_url
{
public:
//: open an URL
// If url is "file://..." open as a file with given mode.
// If url is "http://..." open using vul_http_open
// If url is "ftp://..." attempt ftp
// Otherwise assume it is a filename and open with given mode
  static vcl_istream * open(const char * url, vcl_ios::open_mode mode=vcl_ios::in );

//: Does that URL exist
// If the URL does not begin with a recognised xcheme identifier, the function will
// treat the parameter as a local filename
  static bool exists(const char * url);

//: Is that a URL
// If the URL does not begin with a recognised xcheme identifier, the function will
// treat the parameter as a local filename
  static bool is_url(const char * url);

//: Is that a file
// i.e. is it a downloadable URL, or a file on disk that isn't a directory.
  static bool is_file(const char * url);

//: Encode a string of chars into base64 format
  static vcl_string encode_base64(const vcl_string& in);

//: Decode a string of chars from base64 format
  static vcl_string decode_base64(const vcl_string& in);
};
#endif
