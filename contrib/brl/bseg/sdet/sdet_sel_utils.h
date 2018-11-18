// This is sdet_sel_utils.h
#ifndef sdet_sel_utils_h
#define sdet_sel_utils_h
//:
//\file
//\brief utility functions for the Symbolic edge linking library
//\author Amir Tamrakar
//\date 09/05/06
//
//\verbatim
//  Modifications
//  Amir Tamrakar 09/05/06   Moved it from sdet_se1.h to a new file
//
//\endverbatim

#include <iostream>
#include <cmath>
#include <vnl/vnl_math.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_distance.h>

//utility functions

inline int sdet_round(double val)
{
  return (int) std::floor(val+0.5);
}

//: Convert an angle to [0, 2Pi) range
inline double sdet_angle0To2Pi (double angle)
{
  double a;
  if (angle>=2*vnl_math::pi)
    a = std::fmod (angle,vnl_math::pi*2);
  else if (angle < 0)
    a = (2*vnl_math::pi+ std::fmod (angle,2*vnl_math::pi));
  else
    a= angle;

  // added by Nhon: these two lines of code is to fix the bug when
  // angle = -1.1721201390607859e-016
  // then after all the computation, we get
  // a = 6.2831853071795862 == 2*vnl_math::pi !!!!!!!
  // the only case this can happen is when a is very close to zero.

  if (!(a>=0 && a<2*vnl_math::pi)) {
    a = 0;
  }

  // assert (a>=0 && a<2*vnl_math::pi);
  return a;
}

//: compute the Counterclockwise angle between the reference and the angle
inline double sdet_CCW (double reference, double angle)
{
   double fangle = sdet_angle0To2Pi(angle);
   double fref = sdet_angle0To2Pi(reference);

   if (fref > fangle){
    return sdet_angle0To2Pi(2*vnl_math::pi - (fref - fangle));
  }
   else
      return sdet_angle0To2Pi(fangle - fref);
}

inline double sdet_vPointPoint (vgl_point_2d<double> start, vgl_point_2d<double> end)
{
    return sdet_angle0To2Pi (std::atan2 (end.y() - start.y(), end.x() - start.x()) );
}

//: dot product between two vectors
inline double sdet_dot (double v1, double v2)
{
   return std::cos(v1)*std::cos(v2) + std::sin(v1)*std::sin(v2);
}

//: this structure holds the intrinsic params for an edge pair
struct sdet_int_params{
  double d;         ///< distance between the edges
  double ref_dir;   ///< vector connecting the two edge points
  double t1;        ///< tangent of edge 1 wrt ref_dir
  double t2;        ///< tangent of edge 1 wrt ref_dir
};

//: get intrinsic parameters for an edge pair
inline sdet_int_params sdet_get_intrinsic_params(vgl_point_2d<double> pt1, vgl_point_2d<double> pt2,
                                      double tan1, double tan2)
{
  sdet_int_params pair_params;

  //determine the intrinsic parameters for this edgel pair
  pair_params.d = vgl_distance(pt1, pt2);
  pair_params.ref_dir = sdet_vPointPoint(pt1, pt2); //reference dir
  pair_params.t1 = sdet_CCW(pair_params.ref_dir, tan1);
  pair_params.t2 = sdet_CCW(pair_params.ref_dir, tan2);

  return pair_params;
}

inline bool sdet_intersect_range(double a1, double b1, double a2, double b2,
                                  double & ai, double & bi)
{
  if (a1<=a2 && a2<=b1){
    ai = a2;
    bi = std::min(b1,b2);
    return true;
  }
  else if (a1<=b2 && b2<=b1){
    ai = std::max(a1, a2);
    bi = b2;
    return true;
  }

  return false; //no intersection
}

// is vector x between a and b(all in the range of 0-2pi)
inline bool sdet_is_between(double x, double a, double b)
{
  //1)Normal case:
  if ( x>a && x<b )
    return true;

  //2)0-2pi crossing case:
  if (b<a)
    if ((x>=0 && x<b) || (x>a && x<=2*vnl_math::pi))
      return true;

  return false;
}

//compute the minimum of the maximums of the two ranges
inline double sdet_angle_min_max(double a1, double b1, double a2, double b2)
{
  if (a1<b1 && b2<a2){ //second range straddles the discontinuity
    //assuming a2 between (a1, b1)
    return b1;
  }
  else if (b1<a1 && a2<b2){ //first range straddles the discontinuity
    //assuming a2 between (a1, b1)
    if (sdet_is_between(b2, a1, b1))
      return b2;
    else
      return b1;
  }
  else
    return std::min(b1, b2);
}

//compute the maximum of the minimums of the two ranges
inline double sdet_angle_max_min(double a1, double b1, double a2, double b2)
{
  if (a1<b1 && b2<a2){ //second range straddles the discontinuity
    //assuming b2 between (a1, b1)
    return a1;
  }
  else if (b1<a1 && a2<b2){ //first range straddles the discontinuity
    //assuming b2 between (a1, b1)
    if (sdet_is_between(a2, a1, b1))
      return a2;
    else
      return a1;
  }
  else
    return std::max(a1, a2);
}

inline bool sdet_intersect_angle_range(double a1, double b1, double a2, double b2,
                                        double & ai, double & bi)
{
  //convert all arguments to 0-2pi range
  double A1 = sdet_angle0To2Pi(a1);
  double B1 = sdet_angle0To2Pi(b1);
  double A2 = sdet_angle0To2Pi(a2);
  double B2 = sdet_angle0To2Pi(b2);

  if (sdet_is_between(A2, A1, B1)){
    ai = A2;
    bi = sdet_angle_min_max(A1, B1, A2, B2);
    return true;
  }
  else if (sdet_is_between(B2, A1, B1)){
    ai = sdet_angle_max_min(A1, B1, A2, B2);
    bi = b2;
    return true;
  }
  else if (sdet_is_between(A1, A2, B2) && sdet_is_between(B1, A2, B2)){
    ai = A1;
    bi = B1;
    return true;
  }

  return false; //no intersection
}

inline bool sdet_intersect_angle_range_mpi_pi(double a1, double b1, double a2, double b2,
                                        double & ai, double & bi)
{
  //convert all arguments to 0-2pi range
  double A1 = sdet_angle0To2Pi(a1);
  double B1 = sdet_angle0To2Pi(b1);
  double A2 = sdet_angle0To2Pi(a2);
  double B2 = sdet_angle0To2Pi(b2);

  bool int_valid = false;

  if (sdet_is_between(A2, A1, B1)){
    ai = A2;
    bi = sdet_angle_min_max(A1, B1, A2, B2);
    int_valid = true;
  }
  else if (sdet_is_between(B2, A1, B1)){
    ai = sdet_angle_max_min(A1, B1, A2, B2);
    bi = b2;
    int_valid = true;
  }
  else if (sdet_is_between(A1, A2, B2) && sdet_is_between(B1, A2, B2)){
    ai = A1;
    bi = B1;
    int_valid = true;
  }

  if (int_valid){
    //use -pi - pi range
    if (ai>vnl_math::pi)
      ai-= 2*vnl_math::pi;
    if (bi>vnl_math::pi)
      bi-= 2*vnl_math::pi;
  }

  return int_valid; //no intersection
}

#endif // sdet_sel_utils_h
