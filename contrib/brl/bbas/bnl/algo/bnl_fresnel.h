// This is bbas/bnl/bnl_fresnel.h
#ifndef bnl_fresnel_h_
#define bnl_fresnel_h_

//:
// \file
// \brief Fresnel integrals - sine and cosine
//
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date 2/03/05
//
// \verbatim
//  Modifications:
// \endverbatim

// Reference: book "Numerical Recipes in C" by William Press et.al. Section 6.9, page 255


//: Fresnel cosine integral
// FresnelC = int(cos(pi/2 * t^2), t = 0..x);
// FresnelS = int(sin(pi/2 * t^2), t = 0..x);
// Return: fs is Fresnel cosin integral, fc is Frenel sine integral
void bnl_fresnel_integral(double x, double* fresnel_cos, double* fresnel_sin);

#endif // bbas/bnl/bnl_fresnel.h
