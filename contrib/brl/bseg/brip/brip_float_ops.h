#ifndef brip_float_ops_h_
#define brip_float_ops_h_

//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief operations on memory_image_of<float> operands
//
// Thse methods are similar to the VanDuc gevd_float_ops methods. However,
// they use vil_image_of<float> buffers rather than the old bufferxy 
// structure. The purpose is to provide efficient foundational
// segmentation routines. They are not meant to be generic.
//
// \verbatim
// Initial version February 15, 2003
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vbl/vbl_array_2d.h>
#include <vil/vil_memory_image_of.h>

class brip_float_ops
{
 public:
  ~brip_float_ops(){};

  //:convolves with the specified kernel
  static vil_memory_image_of<float>  
    convolve(vil_memory_image_of<float> const & input,
             vbl_array_2d<float> const & kernel);

  //:convolves with the specified kernel
  static vil_memory_image_of<float>  
    gaussian(vil_memory_image_of<float> const & input, float sigma);

  //:subtracts image_1 from image_2
  static vil_memory_image_of<float>  
    difference(vil_memory_image_of<float> const & image_1, 
               vil_memory_image_of<float> const & image_2); 

  //: The gradient using a 3x3 kernel
  static void gradient_3x3(vil_memory_image_of<float> const & input,
                           vil_memory_image_of<float>& grad_x,
                           vil_memory_image_of<float>& grad_y);

  //:computes the conditioning of the 2n+1 x 2n+1 gradient neigborhood
  static vil_memory_image_of<float>  
    sqrt_grad_singular_values(vil_memory_image_of<float> & input, int n);
  
  //:fills a border of width w on left and right of image with value
  static void fill_x_border(vil_memory_image_of<float> & image,
                          int w, float value);

  //:fills a border of width h on top and bottom of image with value
  static void fill_y_border(vil_memory_image_of<float> & image,
                            int h, float value);

  //:converts a float image to a byte value range
  static vil_memory_image_of<unsigned char> 
    convert_to_byte(vil_memory_image_of<float> const & image);

  //:converts a float image to a byte value range within a specified range
  static vil_memory_image_of<unsigned char> 
    convert_to_byte(vil_memory_image_of<float> const & image,
                    const float min_val, const float max_val);

  //:converts a byte image to a float image
  static vil_memory_image_of<float>
    convert_to_float(vil_memory_image_of<unsigned char> const & image);

  //:loads a 2n+1 x 2n+1 convolution kernel (see .cxx for file format)
  static vbl_array_2d<float> load_kernel(vcl_string const & file);
 private:
  brip_float_ops(){};
};

#endif
