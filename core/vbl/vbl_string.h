#ifndef vbl_string_h
#define vbl_string_h

// DESCRIPTION
// This class provides some utility functions for C-strings 
// and vcl_string's.

#include <vcl/vcl_string.h>

// C-string functions:
extern char* vbl_string_c_upcase(char*);
extern char* vbl_string_c_downcase (char*);
extern char* vbl_string_c_capitalize (char*);
extern char* vbl_string_c_trim (char*, const char*); 
extern char* vbl_string_c_left_trim (char*, const char*);
extern char* vbl_string_c_right_trim (char*, const char*);
extern void vbl_string_c_reverse(char*);

// vcl_string functions:
extern vcl_string& vbl_string_upcase(vcl_string&);
extern vcl_string& vbl_string_downcase(vcl_string&);
extern vcl_string& vbl_string_capitalize(vcl_string&);
extern vcl_string& vbl_string_trim(vcl_string&, const char*);
extern vcl_string& vbl_string_left_trim(vcl_string&, const char*);
extern vcl_string& vbl_string_right_trim(vcl_string&, const char*);

// from GenString
extern void vbl_string_compile (const char*);           // Compile regexp in argument
extern bool vbl_string_find (vcl_string&);                  // Search for first/next regexp
extern long vbl_string_start ();                 // Return start index of match
extern long vbl_string_end ();                   // Return end index of match
 
extern int  vbl_string_atoi(vcl_string&);

#endif
