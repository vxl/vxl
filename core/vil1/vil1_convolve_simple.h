// This is core/vil1/vil1_convolve_simple.h
#ifndef vil1_convolve_simple_h_
#define vil1_convolve_simple_h_
//:
// \file
// \brief Simple convolution functions
// \author awf@robots.ox.ac.uk

template <class T> class vil1_memory_image_of;

//: Convolution
// This function assumes that the image types provide operator[] to
// return a pointer (or something which behaves like one) to the y'th
// raster.
//
// The given regions of the input images are convolved and placed at
// the given position in the output image, according to the formula :
// $\displaystyle OUT[y][x] = \sum_{x_1+x_2=x, y_1+y_2=y} IN_1[y_1][x_1] IN_2[y_2][x_2]$
//
// The dummy parameter is just for dumb compilers that do not allow
// the use of vil1_convolve_simple<...> to specify the accumulator type.
//
// -  I1 : input image type
// -  I2 : input image type
// -  AC : accumulator pixel type
// -  O  : output image type

template <class I1, class I2, class AC, class O>
void vil1_convolve_simple(I1 const* const*  in1, unsigned w1, unsigned h1, // input 1
                          I2 const* const*  in2, unsigned w2, unsigned h2, // input 2
                          AC *, // dummy
                          O       * const*  out);                          // output

//: Convolution
// This function performs some bounds checks on the given memory images
// and then dispatches the convolution itself to the above function.
//
// - I1 : input image type
// - I2 : input image type
// - AC : accumulator pixel type
// - O  : output image type
//
// $\displaystyle {\rm out}_{{\rm off}+k} = \sum_{i+j=k} a_{{\rm off}+i} b_{{\rm off}+j}$

template <class I1, class I2, class AC, class O>
void vil1_convolve_simple(// input 1
                          vil1_memory_image_of<I1> const &IN1, // input 1
                          int x1, int y1, unsigned w1, unsigned h1,
                          vil1_memory_image_of<I2> const &IN2, // input 2
                          int x2, int y2, unsigned w2, unsigned h2,
                          AC *, //dummy
                          vil1_memory_image_of<O>        &OUT, // output
                          int xo, int yo);

//: Convolution
// Like the previous function, except without bounds checking and
// specified regions.
//
// - I1 : input image type
// - I2 : input image type
// - AC : accumulator pixel type
// - O  : output image type
//
// $\displaystyle {\rm out}_{{\rm off}+k} = \sum_{i+j=k} a_{{\rm off}+i} b_{{\rm off}+j}$

template <class I1, class I2, class AC, class O>
void vil1_convolve_simple(vil1_memory_image_of<I1> const &input1,
                          vil1_memory_image_of<I2> const &input2,
                          AC *, // dummy
                          vil1_memory_image_of<O>        &output);

#endif // vil1_convolve_simple_h_
