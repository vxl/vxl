#include <math.h>

#ifndef _INLINES_H__
#define _INLINES_H__

inline bool epsilon_equals(double d1, double d2, double epsilon=1.0e-6)
{
	if (fabs(d1-d2)<epsilon) return(true);
	else return(false);
}

#endif
