//:
// \file
// \brief Example of using vil_convolve_1d with a vil_image_view<T> to smooth an image with a Gaussian.
// \author Ian Scott, David Serby

#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_transpose.h>
#include <vil/algo/vil_convolve_1d.h>


int main( int argc, char* argv[] )
{
  // get file name
  char const* inputFilename = argc>1 ? argv[1] : "input file: please specify 2 command line arguments";
  char const* outputFilename = argc>2 ? argv[2] : "output_file.pnm";

  // load image --> input image
  vil_image_view<vxl_byte> imageIn;
  imageIn = vil_load( inputFilename );
  if (!imageIn)
  {
    vcl_cout << "Unable to correctly load " << inputFilename << vcl_endl;
    return 2;
  }

  // create an output image and a temporary image of the same size as the
  // input image
  vil_image_view<vxl_byte> imageOut( imageIn.ni(), imageIn.nj() );
  vil_image_view<float> tmp( imageIn.ni(), imageIn.nj() );

  // create a normalized Gaussian kernel with standard deviation sigma=2.0
  vcl_cout << "Creating kernel...\n";
  float sigma = 2.f;
  float scaleFactor = 1.f / ( 2.f * sigma * sigma );
  const int halfSupport = 1;

  // build kernel
  float kernel[2*halfSupport+1];
  float sum = 0;
  for ( int i = -halfSupport ; i <= halfSupport ; ++i )
  {
    kernel[i+halfSupport] = vcl_exp( -i * i * scaleFactor );
    sum += kernel[i+halfSupport];
  }

  // normalize kernel
  for ( int i = -halfSupport ; i <= halfSupport ; ++i )
  {
    kernel[i+halfSupport] /= sum;
  }

  // convolution of the input image in y-direction
  vcl_cout << "Convolving in y-direction...\n";
  vil_convolve_1d(vil_transpose( imageIn ), tmp,
                  &kernel[halfSupport], -halfSupport, halfSupport,
                  float(),
                  vil_convolve_constant_extend,
                  vil_convolve_constant_extend );

  // convolution of the input image in x-direction
  vcl_cout << "Convolving in x-direction...\n";
  vil_convolve_1d(vil_transpose( tmp ), imageOut,
                  &kernel[1], -halfSupport, halfSupport,
                  float(),
                  vil_convolve_constant_extend,
                  vil_convolve_constant_extend );

  vil_save(imageOut, outputFilename);
  return 0;
}

