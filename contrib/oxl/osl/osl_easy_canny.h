// This is oxl/osl/osl_easy_canny.h
#ifndef osl_easy_canny_h_
#define osl_easy_canny_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
// .NAME osl_easy_canny
// .INCLUDE osl/osl_easy_canny.h
// .FILE osl_easy_canny.cxx
// \author fsm@robots.ox.ac.uk

#include <vcl_list.h>
class osl_edge;
class vil_image;

// 0: oxford
// 1: rothwell1
// 2: rothwell2
void osl_easy_canny(int which_canny,
                    vil_image const &image,
                    vcl_list<osl_edge*> *edges,
                    double sigma = 0);

#endif // osl_easy_canny_h_
