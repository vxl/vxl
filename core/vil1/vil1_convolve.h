#ifndef vil_convolve_h_
#define vil_convolve_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

template <class T> class vil_memory_image_of;

// This function assumes that the image types provide operator[] to
// return a pointer (or something which behaves like one) to the y'th
// raster.
//
// The given regions of the input images are convolved and placed at
// the given position in the output image, according to the formula :
//   OUT[y][x] = \sum_{x1+x2=x, y1+y2=y} IN1[y1][x1] IN2[y2][x2]
//
// The dummy parameter is just for dumb compilers that do not allow
// the use of vil_convolve_simple<...> to specify the accumulator type.
//
// I1 : input image type
// I2 : input image type
// AC : accumulator pixel type
// O  : output image type
template <class I1, class I2, class AC, class O>
void vil_convolve_simple(I1 const* const*  in1, unsigned w1, unsigned h1, // input 1
			 I2 const* const*  in2, unsigned w2, unsigned h2, // input 2
			 O       * const*  out,                           // ouput
			 AC *); // dummy

// This function performs some bounds checks on the given memory images
// and the dispatches the convolution itself to the above function.
//
// I1 : input pixel type
// I2 : input pixel type
// AC : accumulator pixel type
// O  : output pixel type
//
// out_{off+k} = \sum_{i+j = k} a_{off+i} b_{off+j}
template <class I1, class I2, class AC, class O>
void vil_convolve_simple(// input 1
			 vil_memory_image_of<I1> const &IN1, // input 1
			 int x1, int y1, unsigned w1, unsigned h1,
			 vil_memory_image_of<I2> const &IN2, // input 2
			 int x2, int y2, unsigned w2, unsigned h2,
			 vil_memory_image_of<O>        &OUT, // output
			 int xo, int yo,
			 AC *); // dummy

// Like the previous function, except without bounds checking and
// specified regions.
//
// out_{off+k} = \sum_{i+j = k} a_{off+i} b_{off+j}
template <class I1, class I2, class AC, class O>
void vil_convolve_simple(vil_memory_image_of<I1> const &input1,
			 vil_memory_image_of<I2> const &input2,
			 vil_memory_image_of<O>        &output,
			 AC *); // dummy

#endif
