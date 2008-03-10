// This is contrib/brl/bbas/bil/algo/bil_color_conversions.h
#ifndef bil_color_conversions_h_
#define bil_color_conversions_h_
//:
// \file
// \brief Functions to convert between color spaces
// \author Amir Tamrakar
//
// \verbatim
//  Modifications
//   O.C.Ozcanli  02/18/2007   added the distance method in LAB space
//
// \endverbatim 


#include <vil/vil_image_view.h>
#include <vnl/vnl_vector_fixed.h>

//functions for converting an RGB image to a CIE-Lab image
void rgb2lab(double R,double G, double B, double &L, double &a, double &b);
void convert_RGB_to_Lab(vil_image_view<unsigned char> const& image,
                        vil_image_view<float>& L_img,
                        vil_image_view<float>& a_img,
                        vil_image_view<float>& b_img);

void convert_RGB_to_Lab(vil_image_view<vxl_byte> const& image,
                        vil_image_view<double>& L_img,
                        vil_image_view<double>& a_img,
                        vil_image_view<double>& b_img);

//functions for converting an RGB image to a CIE-Luv image
void rgb2luv(double R, double G, double B, double &L, double &u, double &v);
void convert_RGB_to_Luv(vil_image_view<unsigned char> const& image,
                        vil_image_view<float>& L_img,
                        vil_image_view<float>& u_img,
                        vil_image_view<float>& v_img);

//: The following distance measure in LAB space is taken from Ruzon, Tomasi PAMI 2001 (Edge, Junction and Corner Detection..)
double distance_LAB(double L1, double A1, double B1, double L2, double A2, double B2, double color_gamma);
double distance_LAB(vnl_vector_fixed<double, 3>& c1, vnl_vector_fixed<double, 3>& c2, double color_gamma);

double distance_intensity(double plus_mean, double minus_mean, double intensity_gamma);

#endif // bil_color_conversions_h_

