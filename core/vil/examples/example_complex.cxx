//:
// \file
// \brief Examples using complex images
// \author Fred Wheeler

#include <vxl_config.h>
#include <vcl_iostream.h>
#include <vcl_complex.h>
#include <vil2/vil2_image_view.h>
#include <vil2/vil2_view_as.h>
#include <vil2/vil2_print.h>
#include <vil2/vil2_convert.h>
#include <vil2/vil2_save.h>
#include <vil2/algo/vil2_fft.h>

int
main (int argc, char** argv)
{
  vil2_image_view<vcl_complex<double> > img (4, 2, 3);

  // fill in the complex image view
  for (unsigned i=0; i<img.ni(); i++)
  for (unsigned j=0; j<img.nj(); j++)
  for (unsigned p=0; p<img.nplanes(); p++)
      img (i, j, p) =
          vcl_complex<double> (111+i+10*j+100*p+0.1, 111+i+10*j+100*p+0.2);

  // print the complex image view
  vcl_cout << "original complex image" << vcl_endl;
  vil2_print_all (vcl_cout, img);
  vcl_cout << vcl_endl;

  // view as planes
  // this only works if the complex image has only 1 plane
  vil2_image_view<double> img_p = vil2_view_as_planes (img);

  // print the plane image view
  vcl_cout << "complex image viewed as planes" << vcl_endl;
  vil2_print_all (vcl_cout, img_p);
  vcl_cout << vcl_endl;

  // convert plane view back to complex view
  vil2_image_view<vcl_complex<double> > img_c = vil2_view_as_complex (img_p);

  // print the plane-complex image view
  vcl_cout << "plane view converted back to complex view" << vcl_endl;
  vil2_print_all (vcl_cout, img_c);
  vcl_cout << vcl_endl;

  // get separate views of the real and imaginary parts of the image
  vil2_image_view<double> img_r = vil2_view_real_part (img);
  vil2_image_view<double> img_i = vil2_view_imag_part (img);

  // print the separate views of the real and imaginary parts
  vcl_cout << "view of real part" << vcl_endl;
  vil2_print_all (vcl_cout, img_r);
  vcl_cout << vcl_endl;
  vcl_cout << "view of imag part" << vcl_endl;
  vil2_print_all (vcl_cout, img_i);
  vcl_cout << vcl_endl;

  // change values via the separate views
  img_r(0,0,0) = 888.8;
  img_i(1,1,0) = 999.9;

  // print the complex image view again to see the change
  vcl_cout << "complex image changed via real/imag parts" << vcl_endl;
  vil2_print_all (vcl_cout, img);
  vcl_cout << vcl_endl;

  // make a copy
  vil2_image_view<vcl_complex<double> > img2;
  img2.deep_copy (img);

  // forward FFT
  vil2_fft_2d_fwd (img2);

  // print the complex image view again to see the change
  vcl_cout << "FFT of complex image" << vcl_endl;
  vil2_print_all (vcl_cout, img2);
  vcl_cout << vcl_endl;

  // backward FFT
  vil2_fft_2d_bwd (img2);

  // print the complex image view again to see the change
  vcl_cout << "inverse FFT of FFT of complex image" << vcl_endl;
  vil2_print_all (vcl_cout, img2);
  vcl_cout << vcl_endl;

  // convert real part to an integral type
  vil2_image_view<vxl_uint_16> img_ri = img_r;
  vil2_convert_cast (img_r, img_ri);

  vcl_cout << "cast real part to an integer image" << vcl_endl;
  vil2_print_all (vcl_cout, img_ri);
  vcl_cout << vcl_endl;

  vil2_save (img_ri, "foo.png");

  return 0;
}
