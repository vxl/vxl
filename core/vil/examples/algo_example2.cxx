//:
// \file
// \brief Example of using vil2_convolve_1d with a vil2_image_view<T> to smooth an image with a Gaussian.
// \author Ian Scott, David Serby


#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vil2/vil2_image_view.h>
#include <vil2/vil2_load.h>
#include <vil2/vil2_save.h>
#include <vil2/vil2_transpose.h>
#include <vil2/algo/vil2_convolve_1d.h>


int main( int argc, char* argv[] )
{
  // get file name
  char const* inputFilename = argv[1];
  char const* outputFilename = argv[2];

  // load image --> input image
  vil2_image_view<vxl_byte> imageIn;
  imageIn = vil2_load( inputFilename );
  if (!imageIn)
  {
    vcl_cout << "Unable to correctly load " << inputFilename << vcl_endl;
    return 2;
  }


  // create an output image and a temporary image of the same size as the
  // input image
  vil2_image_view<vxl_byte> imageOut( imageIn.ni(), imageIn.nj() );
  vil2_image_view<float> tmp( imageIn.ni(), imageIn.nj() );

  // create a normalized Gaussian kernel with standard deviation sigma=2.0
  vcl_cout << "Creating kernel..." << vcl_endl;
  float sigma = 2.0;
  float scaleFactor = 1.0 / ( 2.0 * sigma * sigma );
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
  vcl_cout << "Convolving in y-direction..." << vcl_endl;
  vil2_convolve_1d( vil2_transpose( imageIn ), tmp,
                 &kernel[halfSupport], -halfSupport, halfSupport,
                 float(),
                 vil2_convolve_constant_extend,
                 vil2_convolve_constant_extend );

  // convolution of the input image in x-direction
  vcl_cout << "Convolving in x-direction..." << vcl_endl;
  vil2_convolve_1d( vil2_transpose( tmp ), imageOut,
                 &kernel[1], -halfSupport, halfSupport,
                 float(),
                 vil2_convolve_constant_extend,
                 vil2_convolve_constant_extend );

  vil2_save(imageOut, outputFilename);
  return 0;
}

