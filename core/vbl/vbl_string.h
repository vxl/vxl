#ifndef vbl_string_h
#define vbl_string_h

// .NAME vbl_string - Utility functions for C strings and vcl_strings
// .HEADER vxl package
// .LIBRARY vbl
// .INCLUDE vbl/vbl_string.h
// .FILE vbl_string.cxx

#include <vcl/vcl_string.h>

// C string functions:
extern char* vbl_string_c_upcase(char*);
extern char* vbl_string_c_downcase (char*);
extern char* vbl_string_c_capitalize (char*);
extern char* vbl_string_c_trim (char*, const char*); 
extern char* vbl_string_c_left_trim (char*, const char*);
extern char* vbl_string_c_right_trim (char*, const char*);
extern void  vbl_string_c_reverse(char*);

// vcl_string functions:
extern vcl_string& vbl_string_upcase(vcl_string&);
extern vcl_string& vbl_string_downcase(vcl_string&);
extern vcl_string& vbl_string_capitalize(vcl_string&);
extern vcl_string& vbl_string_trim(vcl_string&, const char*);
extern vcl_string& vbl_string_left_trim(vcl_string&, const char*);
extern vcl_string& vbl_string_right_trim(vcl_string&, const char*);
 
extern int vbl_string_atoi(vcl_string const&);

#endif // vbl_string_h
