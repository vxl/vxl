#ifndef vil_smooth_h_
#define vil_smooth_h_
#ifdef __GNUC__
#pragma interface "vil_smooth"
#endif
/*
  fsm@robots.ox.ac.uk
*/

#include <vil/vil_image.h>

// the kernel has size N.
vil_image vil_smooth(float const kernel[], unsigned N,
		     vil_image const &);

#endif
