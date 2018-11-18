// This is brl/bbas/bil/algo/bil_color_conversions.cxx
#include <iostream>
#include "bil_color_conversions.h"
//:
// \file
#include <vnl/vnl_vector_fixed.h>
#if 0
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#endif

void rgb2lab(double R, double G, double B, double &L , double &a, double &b)
{
  //1) First convert RGB (assuming sRGB) to 1931 CIE-XYZ space

  //convert to [0,1] range
  double var_R = (R/255.0);   //R = From 0 to 255
  double var_G = (G/255.0);   //G = From 0 to 255
  double var_B = (B/255.0);   //B = From 0 to 255

  if ( var_R > 0.04045 ) var_R = std::pow( (var_R+0.055)/1.055, 2.4);
  else                   var_R = var_R /12.92;

  if ( var_G > 0.04045 ) var_G = std::pow( (var_G+0.055)/1.055, 2.4);
  else                   var_G = var_G /12.92;

  if ( var_B > 0.04045 ) var_B = std::pow( (var_B+0.055)/1.055, 2.4);
  else                   var_B = var_B /12.92;

  double X = var_R * 0.4124 + var_G * 0.3576 + var_B * 0.1805;
  double Y = var_R * 0.2126 + var_G * 0.7152 + var_B * 0.0722;
  double Z = var_R * 0.0193 + var_G * 0.1192 + var_B * 0.9505;

  //2) now convert 1931 CIE-XYZ to 1976 CIE-Lab
  // Assuming Observer = 2 degrees, Illuminant = D65

  double var_X = X * 100.0/95.047;
  double var_Y = Y * 100.0/100.000;
  double var_Z = Z * 100.0/108.883;

  if ( var_X > 0.008856 ) var_X = std::pow(var_X, 1.0/3.0);
  else                    var_X = 7.787 * var_X + 16.0/116.0;

  if ( var_Y > 0.008856 ) var_Y = std::pow(var_Y ,1.0/3.0);
  else                    var_Y = 7.787 * var_Y + 16.0/ 116.0;

  if ( var_Z > 0.008856 ) var_Z = std::pow(var_Z, 1.0/3.0);
  else                    var_Z = 7.787 * var_Z + 16.0/ 116.0;

  //Finally
  L = 116.0 * var_Y  - 16.0;
  a = 500.0 * ( var_X - var_Y );
  b = 200.0 * ( var_Y - var_Z );
}

void convert_RGB_to_Lab(vil_image_view<vxl_byte> const& image,
                        vil_image_view<double>& L_img,
                        vil_image_view<double>& a_img,
                        vil_image_view<double>& b_img)
{
  unsigned w = image.ni(), h = image.nj();
  L_img.set_size(w,h);
  a_img.set_size(w,h);
  b_img.set_size(w,h);
  for (unsigned r = 0; r < h; r++){
    for (unsigned c = 0; c < w; c++)
    {
      double L, a, b;
      rgb2lab(image(c,r,0),image(c,r,1),image(c,r,2), L, a, b);
      L_img(c,r) = L;
      a_img(c,r) = a;
      b_img(c,r) = b;
    }
  }
}

void convert_RGB_to_Lab(vil_image_view<unsigned char> const& image,
                        vil_image_view<float>& L_img,
                        vil_image_view<float>& a_img,
                        vil_image_view<float>& b_img)
{
  unsigned w = image.ni(), h = image.nj();
  L_img.set_size(w,h);
  a_img.set_size(w,h);
  b_img.set_size(w,h);
  for (unsigned r = 0; r < h; r++){
    for (unsigned c = 0; c < w; c++)
    {
      double L, a, b;
      rgb2lab(image(c,r,0),image(c,r,1),image(c,r,2), L, a, b);
      L_img(c,r) = (float)L;
      a_img(c,r) = (float)a;
      b_img(c,r) = (float)b;
    }
  }
}


void rgb2luv(double R, double G, double B, double &L , double &u, double &v)
{
  //1) First convert RGB (assuming sRGB) to 1931 CIE-XYZ space

  //convert to [0,1] range
  double var_R = (R/255.0);   //R = From 0 to 255
  double var_G = (G/255.0);   //G = From 0 to 255
  double var_B = (B/255.0);   //B = From 0 to 255

  if ( var_R > 0.04045 ) var_R = std::pow( (var_R+0.055)/1.055, 2.4);
  else                   var_R = var_R /12.92;

  if ( var_G > 0.04045 ) var_G = std::pow( (var_G+0.055)/1.055, 2.4);
  else                   var_G = var_G /12.92;

  if ( var_B > 0.04045 ) var_B = std::pow( (var_B+0.055)/1.055, 2.4);
  else                   var_B = var_B /12.92;

  double X = var_R * 0.4124 + var_G * 0.3576 + var_B * 0.1805;
  double Y = var_R * 0.2126 + var_G * 0.7152 + var_B * 0.0722;
  double Z = var_R * 0.0193 + var_G * 0.1192 + var_B * 0.9505;

  //2) now convert 1931 CIE-XYZ to 1976 CIE-Luv
  // Assuming Observer = 2 degrees, Illuminant = D65

  double var_X = X * 100.0/95.047;
  double var_Y = Y * 100.0/100.000;
  double var_Z = Z * 100.0/108.883;

  double u_prime = 4.0*var_X/(var_X + 15.0*var_Y + 3*var_Z);
  double v_prime = 9.0*var_X/(var_X + 15.0*var_Y + 3*var_Z);

  //Finally
  L = 116.0 * std::pow(var_Y ,1.0/3.0)  - 16.0;
  u = 13.0 * L * (u_prime - 0.2009);
  v = 200.0 * L* (v_prime - 0.4610);
}

void convert_RGB_to_Luv(vil_image_view<unsigned char> const& image,
                        vil_image_view<float>& L_img,
                        vil_image_view<float>& u_img,
                        vil_image_view<float>& v_img)
{
  unsigned w = image.ni(), h = image.nj();
  L_img.set_size(w,h);
  u_img.set_size(w,h);
  v_img.set_size(w,h);
  for (unsigned r = 0; r < h; r++){
    for (unsigned c = 0; c < w; c++)
    {
      double L, u, v;
      rgb2luv(image(c,r,0),image(c,r,1),image(c,r,2), L, u, v);
      L_img(c,r) = (float)L;
      u_img(c,r) = (float)u;
      v_img(c,r) = (float)v;
    }
  }
}

double distance_LAB(double L1, double A1, double B1, double L2, double A2, double B2, double color_gamma)
{
  vnl_vector_fixed<double, 3> c1, c2;
  c1[0] = L1; c1[1] = A1; c1[2] = B1;
  c2[0] = L2; c2[1] = A2; c2[2] = B2;
  return distance_LAB(c1, c2, color_gamma);
}

//: The following distance measure in LAB space is taken from Ruzon, Tomasi PAMI 2001 (Edge, Junction and Corner Detection..)
double distance_LAB(vnl_vector_fixed<double, 3>& c1, vnl_vector_fixed<double, 3>& c2, double color_gamma)
{
  vnl_vector_fixed<double, 3> sub = c1 - c2;
  double E = sub.two_norm();
  // the ground distance between colors is an exponential measure, with steepness governed by gamma (gamma = 14 in Tomasi PAMI 01)
#if 0
  std::cout << "APP E (Euclidean dist): " << E << " [e^(-E/gamma)]: " << std::exp(-E/color_gamma) << std::endl;
#endif
  return std::exp(-E/color_gamma);
}

double distance_intensity(double plus_mean, double minus_mean, double intensity_gamma)
{
  double E = std::abs(plus_mean - minus_mean);
  return std::exp(-E/intensity_gamma);
}
