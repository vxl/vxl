#ifndef __TBS_UTILS_DP_DEF__
#define __TBS_UTILS_DP_DEF__
//:
// \file

//: return the maximum of three numbers
//double maxof(double a, double b, double c);
//: return true if the difference between a and b is smaller than 1e-4
bool almostEqual(double a, double b);
//: return true if the difference between a and b is smaller than eps
bool almostEqual(double a, double b, double eps);
//: return a modulo 2*pi, in the interval -pi to pi
double fixAngleMPiPi(double a);
//: return a modulo 2*pi, in the interval 0 to 2*pi
double fixAngleZTPi(double a);
//: return a1-a2 modulo 2*pi, in the interval -pi to pi
inline double angleDiff(double a1, double a2) { return fixAngleMPiPi(a1-a2); }
//: return a1+a2 modulo 2*pi, in the interval -pi to pi
inline double angleAdd(double a1, double  a2) { return fixAngleMPiPi(a1+a2); }
//: ??
double angleFixForAdd(double ref, double  a);
//inline double pointDist(vgl_point_2d<double> a ,vgl_point_2d<double> b) { return vgl_distance(a,b); }

#endif
