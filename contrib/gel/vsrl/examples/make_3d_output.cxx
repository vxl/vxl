// gtk-display-two-images
// We wish to display two images side by side.

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vsrl/vsrl_results_dense_matcher.h>
#include <vsrl/vsrl_3d_output.h>

#include <vil/vil_image.h>
#include <vil/vil_load.h>
#include <vsrl/vsrl_parameters.h>

// we want to make a 3D dataset from a given set of data points


int main(int argc, char ** argv)
{
  if (argc < 2) {
    vcl_cout << "make_3d_output im1 im2 disp_image 3d_outfile (params.dat) (transform.dat)\n";
    return 0;
  }

  // load the two images

  vil_image image1 = vil_load(argv[1]);
  vil_image image2 = vil_load(argv[2]);

  // load the disparity image

  vil_image disp_image = vil_load(argv[3]);

  vsrl_results_dense_matcher matcher(image1,disp_image);

  // set the correlation range to +/- 10 pixels

  vsrl_parameters *params = vsrl_parameters::instance();

  if (argc>5) {
    params->load(argv[5]);
  }

  matcher.set_correlation_range(params->correlation_range);

  // make a vsrl_3d_output object(image1,image2);

  vsrl_3d_output output(image1,image2);

  // set the matcher
  output.set_matcher(&matcher);

  // set the projective transform if it exists
  if (argc==7) {
    output.read_projective_transform(argv[6]);
  }

  // write the output of the data
  output.write_output(argv[4]);
  return 0;
}
