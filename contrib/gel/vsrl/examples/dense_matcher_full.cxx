// gtk-display-two-images
// We wish to display two images side by side.

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vsrl/vsrl_stereo_dense_matcher.h>
#include <vsrl/vsrl_parameters.h>

#include <vil/vil_image.h>
#include <vil/vil_load.h>

int main(int argc, char ** argv)
{
  if (argc<2) {
    vcl_cout << "gui_dense_matcher im1 im2 disparity_file (params_file)\n";
    return 0;
  }

  // create the stereo dense matcher
  vil_image image1 = vil_load(argv[1]);
  vil_image image2 = vil_load(argv[2]);

  // set the parameters of the dense matcher
  vsrl_parameters *params = vsrl_parameters::instance();

  if (argc==5) {
    // we want to set some parameters based from a data file
    params->load(argv[4]);
  }

  vsrl_stereo_dense_matcher matcher(image1,image2);

  // set the correlation range
  matcher.set_correlation_range(params->correlation_range);

  // perform a full dynamic program
  matcher.execute();

  // save the disparity results
  matcher.write_disparity_image(argv[3]);

  return 1;
}
