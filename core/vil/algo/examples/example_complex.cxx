//:
// \file
// \brief Examples using complex images
// \author Fred Wheeler

#include <iostream>
#include <complex>
#include <vxl_config.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view.h>
#include <vil/vil_view_as.h>
#include <vil/vil_print.h>
#include <vil/vil_convert.h>
#include <vil/vil_save.h>
#include <vil/algo/vil_fft.h>

int main()
{
  vil_image_view<std::complex<double> > img (4, 2, 3);

  // fill in the complex image view
  for (unsigned i=0; i<img.ni(); i++)
  for (unsigned j=0; j<img.nj(); j++)
  for (unsigned p=0; p<img.nplanes(); p++)
      img (i, j, p) =
          std::complex<double> (111+i+10*j+100*p+0.1, 111+i+10*j+100*p+0.2);

  // print the complex image view
  std::cout << "original complex image\n";
  vil_print_all (std::cout, img);
  std::cout << std::endl;

  // view as planes
  // this only works if the complex image has only 1 plane
  vil_image_view<double> img_p = vil_view_as_planes (img);

  // print the plane image view
  std::cout << "complex image viewed as planes\n";
  vil_print_all (std::cout, img_p);
  std::cout << std::endl;

  // convert plane view back to complex view
  vil_image_view<std::complex<double> > img_c = vil_view_as_complex (img_p);

  // print the plane-complex image view
  std::cout << "plane view converted back to complex view\n";
  vil_print_all (std::cout, img_c);
  std::cout << std::endl;

  // get separate views of the real and imaginary parts of the image
  vil_image_view<double> img_r = vil_view_real_part (img);
  vil_image_view<double> img_i = vil_view_imag_part (img);

  // print the separate views of the real and imaginary parts
  std::cout << "view of real part\n";
  vil_print_all (std::cout, img_r);
  std::cout << "\nview of imag part\n";
  vil_print_all (std::cout, img_i);
  std::cout << std::endl;

  // change values via the separate views
  img_r(0,0,0) = 888.8;
  img_i(1,1,0) = 999.9;

  // print the complex image view again to see the change
  std::cout << "complex image changed via real/imag parts\n";
  vil_print_all (std::cout, img);
  std::cout << std::endl;

  // make a copy
  vil_image_view<std::complex<double> > img2;
  img2.deep_copy (img);

  // forward FFT
  vil_fft_2d_fwd (img2);

  // print the complex image view again to see the change
  std::cout << "FFT of complex image\n";
  vil_print_all (std::cout, img2);
  std::cout << std::endl;

  // backward FFT
  vil_fft_2d_bwd (img2);

  // print the complex image view again to see the change
  std::cout << "inverse FFT of FFT of complex image\n";
  vil_print_all (std::cout, img2);
  std::cout << std::endl;

  // convert real part to an integral type
  vil_image_view<vxl_uint_16> img_ri = img_r;
  vil_convert_cast (img_r, img_ri);

  std::cout << "cast real part to an integer image\n";
  vil_print_all (std::cout, img_ri);
  std::cout << std::endl;

  vil_save (img_ri, "foo.png");

  return 0;
}
