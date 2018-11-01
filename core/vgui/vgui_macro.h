// This is core/vgui/vgui_macro.h
#ifndef vgui_macro_h_
#define vgui_macro_h_
//:
// \file
// \author fsm
// \brief  Some handy preprocessor macros commonly used in vgui
//
// Do not include <std::iostream.h> from this file.


//: Handy gl debugging macro.
//  Sprinkle through your code, and it will report any accumulated OpenGL errors
#define vgui_macro_report_errors vgui_macro_error_reporter(__FILE__,__LINE__)


//: Handy for printing warnings stamped with the location of the source line.
// E.g. vgui_macro_warning << "no more megabytes" << endl;
// You will need to include std::iostream to use this.
#define vgui_macro_warning (std::cerr << __FILE__ ", line " << __LINE__ << ": ")


// internals
extern void vgui_macro_error_reporter(char const *FILE, int LINE);

#endif // vgui_macro_h_
