#ifndef vgui_macro_h_
#define vgui_macro_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vgui_macro
// .INCLUDE vgui/vgui_macro.h
// .FILE vgui_macro.cxx
/*
  fsm@robots.ox.ac.uk
*/

// purpose: collect together some handy preprocessor macros commonly
// used in vgui. 

// Do not include <vcl_iostream.h> from this file.


// -- Handy gl debugging macro.  Sprinkle this through your code, and
// it will report any accumulated OpenGL errors.
#define vgui_macro_report_errors vgui_macro_error_reporter(__FILE__,__LINE__)


// -- Handy for printing warnings stamped with the location of the source line.
// E.g. vgui_macro_warning << "no more megabytes" << endl;
// You will need to include vcl_iostream to use this.
#define vgui_macro_warning (vcl_cerr << __FILE__ ", line " << __LINE__ << ": ")


// internals
extern void vgui_macro_error_reporter(char const *FILE, int LINE);

#endif // vgui_macro_h_
