//:
// \file
// \brief Examples of creating images via interpolation of other images
// \author Fred Wheeler

#include <iostream>
#include <iomanip>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "vxl_config.h" // for vxl_byte
#include "vil/vil_image_view.h"
#include "vil/vil_save.h"
#include "vil/vil_print.h"
#include "vil/vil_bilin_interp.h"
#include "vil/vil_bicub_interp.h"
#include "vil/vil_resample_bilin.h"
#include "vil/vil_resample_bicub.h"

int
main(int argc, char **)
{
  if (argc != 1)
  {
    std::cerr << "No arguments needed\n";
    return 1;
  }

  // size of original image to be created
  unsigned ni = 16;
  unsigned nj = 16;
  unsigned np = 3; // 3 plane/color image

  // the original image
  vil_image_view<vxl_byte> image_orig(ni, nj, np);

  // orig image values will stay this far from 0 and 255
  vxl_byte pad = 16;

  // fill in the original image with random values
  for (unsigned p = 0; p < np; p++)
    for (unsigned j = 0; j < nj; j++)
      for (unsigned i = 0; i < ni; i++)
        // there is nothing magic about these numbers
        // this just makes an image that looks random
        image_orig(i, j, p) = vxl_byte((87 + 145 * i + 167 * j + 197 * p * i * j) % (256 - 2 * pad) + pad);

  std::cout << "Original image:\n";
  vil_print_all(std::cout, image_orig);

  const char * fn_orig = "example_interpolate_image_orig.png";
  vil_save(image_orig, fn_orig);
  std::cout << "Wrote original image to " << fn_orig << std::endl;

  // over-sampling factor for interpolated images
  int fact = 20;

  // create and save a bilinearly interpolated over-sampled image
  const char * fn_bilin = "example_interpolate_image_bilin.png";
  vil_image_view<vxl_byte> image_bilin;
  vil_resample_bilin(
    image_orig, image_bilin, -1.0, -1.0, 1.0 / fact, 0.0, 0.0, 1.0 / fact, fact * (ni + 1) + 1, fact * (nj + 1) + 1);
  vil_save(image_bilin, fn_bilin);
  std::cout << "Wrote bilinearly interpolated over-sampled image to " << fn_bilin << std::endl;

  // create and save a bicubicly interpolated over-sampled image
  const char * fn_bicub = "example_interpolate_image_bicub.png";
  vil_image_view<vxl_byte> image_bicub;
  vil_resample_bicub(
    image_orig, image_bicub, -1.0, -1.0, 1.0 / fact, 0.0, 0.0, 1.0 / fact, fact * (ni + 1) + 1, fact * (nj + 1) + 1);
  vil_save(image_bicub, fn_bicub);
  std::cout << "Wrote bicubicly interpolated over-sampled image to " << fn_bilin << std::endl;

  // change over-sampling factor for diagonal scan
  fact = 4;

  std::cout << std::right << std::fixed << std::setprecision(2);

  // scan down the diagonal of the original image
  // at increments, show the bilinear and bicubic interpolation

  for (int k = -fact; k < fact * int(ni) + 1; k++)
  {
    std::cout << "k: " << std::setw(3) << k << "  ";

    double x = double(k) / fact;
    std::cout << "x: " << std::setw(5) << x << "  ";
    double y = double(k) / fact;
    std::cout << "y: " << std::setw(5) << y << "  ";

    double v_bilin = vil_bilin_interp_safe(image_orig, x, y);
    std::cout << "bilin: " << std::setw(6) << v_bilin << "  ";

    double v_bicub = vil_bicub_interp_safe(image_orig, x, y);
    std::cout << "bicub: " << std::setw(6) << v_bicub << "  ";

    // if we are lined up on an original pixel value, print that too
    if (0 == (k % fact))
    {
      int ki = k / fact;
      if (ki >= 0 && ki < int(ni) && ki < int(nj))
      {
        double v_orig = image_orig(ki, ki);
        std::cout << "orig: " << std::setw(3) << v_orig << "  ";
      }
    }

    std::cout << std::endl;
  }

  return 0;
}
