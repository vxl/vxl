// This is oxl/osl/osl_easy_canny.h
#ifndef osl_easy_canny_h_
#define osl_easy_canny_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
// .NAME osl_easy_canny
// .INCLUDE osl/osl_easy_canny.h
// .FILE osl_easy_canny.cxx
// \author fsm

#include <iostream>
#include <list>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
class osl_edge;
class vil1_image;

// 0: oxford
// 1: rothwell1
// 2: rothwell2
void osl_easy_canny(int which_canny,
                    vil1_image const &image,
                    std::list<osl_edge*> *edges,
                    double sigma = 0);

#endif // osl_easy_canny_h_
