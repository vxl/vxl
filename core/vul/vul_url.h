#ifndef vul_url_h_
#define vul_url_h_
#ifdef __GNUC__
#pragma interface 
#endif

#include <vcl_istream.h>
#include <vcl_string.h>


class vul_url
{
public:
//: open an URL
  static vcl_istream * open(const char * url);

//: Does that URL exist
  static bool exists(const char * url);

//: Is that a URL
  static bool is_url(const char * url);

//: Is that a file
// i.e. is it a downloadable URL, or a file on disk that isn't a directory.
  static bool is_file(const char * url);
};
#endif
