#ifndef vcl_iomanip_h_
#define vcl_iomanip_h_
/*
  fsm@robots.ox.ac.uk
*/

#include <vcl/vcl_compiler.h>
// NB. we allow both 'blah' and 'vcl_blah' to mean 'std::blah'.

#if defined(VCL_GCC) || defined(VCL_SGI_CC)
# include <iomanip.h>
# define vcl_iomanip_h_STD(name) /*std*/::name
# define vcl_iomanip_h_USE(name) /*using std::name*/

#else // std-conforming compilers
# include <iomanip>
# define vcl_iomanip_h_STD(name) std::name
# define vcl_iomanip_h_USE(name) using std::name
#endif

//// ws
//#define vcl_ws vcl_iomanip_h_STD(ws)
//vcl_iomanip_h_USE(ws);
// setw
#define vcl_setw vcl_iomani_h_STD(setw)
vcl_iomanip_h_USE(setw);

#endif
