
#ifndef __TBS_UTILS_DP_DEF__
#define __TBS_UTILS_DP_DEF__

#include <vcl_cmath.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_point_2d.h>

double maxof(double a, double b,double c);
double minof(double a, double b);
int almostEqual(double a, double b,double c);
int almostEqual(double a, double b);
double fixAngleMPiPi(double a);
double fixAngleZTPi(double a);
double angleDiff(double a1, double  a2);
double angleAdd(double a1, double  a2);
double angleFixForAdd(double ref, double  a);
double pointDist(vgl_point_2d<double> a ,vgl_point_2d<double> b);



#endif
