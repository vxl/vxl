//:
// \file
// \brief Examples of creating images via interpolation of other images
// \author Fred Wheeler

#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vil2/vil2_image_view.h>
#include <vil2/vil2_save.h>
#include <vil2/vil2_print.h>
#include <vil2/vil2_resample_bilin.h>
#include <vil2/vil2_resample_bicub.h>

int
main (int argc, char** argv)
{
  if (argc != 1) {
      vcl_cerr << "No argumements needed" << vcl_endl;
      return 1;
  }

  // size of original inage to be created
  unsigned ni = 16;
  unsigned nj = 16;
  unsigned np = 3;

  vil2_image_view<vxl_byte> image_orig (ni, nj, np);

  vxl_byte pad = 16;

  for (unsigned p=0; p<np; p++)
  for (unsigned j=0; j<nj; j++)
  for (unsigned i=0; i<ni; i++)
      image_orig(i,j,p) =
          vxl_byte((87+145*i+167*j+197*p*i*j) % (256 - 2*pad) + pad);

  vcl_cout << "Original image:" << vcl_endl;
  vil2_print_all (vcl_cout, image_orig);

  const char * fn_orig = "example_interpolate_image_orig.png";
  vil2_save(image_orig, fn_orig);
  vcl_cout << "Wrote original image to "
           << fn_orig << vcl_endl;

  // over-sampling factor for interpolated images
  double fact = 20.0;

  // create and save a bilinearly interpolated over-sampled image
  const char * fn_bilin = "example_interpolate_image_bilin.png";
  vil2_image_view<vxl_byte> image_bilin;
  vil2_resample_bilin(image_orig, image_bilin,
                      -1.0, -1.0,
                      1.0/fact, 0.0, 0.0, 1.0/fact,
                      int(fact*(ni+1)+1), int(fact*(nj+1)+1));
  vil2_save(image_bilin, fn_bilin);
  vcl_cout << "Wrote bilinearly interpolated over-sampled image to "
           << fn_bilin << vcl_endl;

  // create and save a bicubicly interpolated over-sampled image
  const char * fn_bicub = "example_interpolate_image_bicub.png";
  vil2_image_view<vxl_byte> image_bicub;
  vil2_resample_bicub(image_orig, image_bicub,
                      -1.0, -1.0,
                      1.0/fact, 0.0, 0.0, 1.0/fact,
                      int(fact*(ni+1)+1), int(fact*(nj+1)+1));
  vil2_save(image_bicub, fn_bicub);
  vcl_cout << "Wrote bicubicly interpolated over-sampled image to "
           << fn_bilin << vcl_endl;

  return 0;
}
