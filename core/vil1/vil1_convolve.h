// This is core/vil1/vil1_convolve.h
#ifndef vil1_convolve_h_
#define vil1_convolve_h_
//:
// \file
// \author fsm

#include <vcl_compiler.h>
#include <vil1/vil1_memory_image_of.h>

//: Available options for boundary behavior
// When convolving a finite signal the boundaries may be
// treated in various ways which can often be expressed in terms
// of ways to extend the signal outside its original range.
enum vil1_convolve_boundary_option {
  // Do not to extend the signal, but pad with zeros.
  //     |                               |
  // K                       ----*-------
  // in   ... ---------------------------
  // out  ... --------------------0000000
  vil1_convolve_no_extend,

  // Zero-extend the input signal beyond the boundary.
  //     |                               |
  // K                              ----*--------
  // in   ... ---------------------------000000000000...
  // out  ... ---------------------------
  vil1_convolve_zero_extend,

  // Extend the signal to be constant beyond the boundary
  //     |                               |
  // K                              ----*--------
  // in   ... --------------------------aaaaaaaaaaaaa...
  // out  ... ---------------------------
  vil1_convolve_constant_extend,

  // Extend the signal periodically beyond the boundary.
  //     |                               |
  // K                              ----*--------
  // in   abc...-------------------------abc...------..
  // out  ... ---------------------------
  vil1_convolve_periodic_extend,

  // Extend the signal by reflection about the boundary.
  //     |                               |
  // K                               ----*--------
  // in   ... -------------------...edcbabcde...
  // out  ... ---------------------------
  vil1_convolve_reflect_extend,

  // This one is slightly different. The input signal is not
  // extended in any way, but the kernel is trimmed to allow
  // convolution to proceed up to the boundary and reweighed
  // to keep the total area the same.
  // *** may not work with kernels which take negative values.
  vil1_convolve_trim
};

//: Parameters for convolution
// These structs exist purely to group the parameters to the
// convolution routines. It is not intended that they be
// expressive or even useful in any other context.
//
// Usually, begin <= origin < end. Expect assertion failures
// if that is not the case.
template <class T>
struct vil1_convolve_signal_1d {
  T *array_;
  int begin_;
  int origin_;
  int end_;
  vil1_convolve_signal_1d(T *a, int b, int o, int e)
    : array_(a), begin_(b), origin_(o), end_(e) { }
};

//: Parameters for convolution
template <class T>
struct vil1_convolve_signal_2d {
  T * const *array_;
  int beginx_, originx_, endx_;
  int beginy_, originy_, endy_;
  vil1_convolve_signal_2d(T * const *a,
                          int bx, int ox, int ex,
                          int by, int oy, int ey)
    : array_(a)
    , beginx_(bx), originx_(ox), endx_(ex)
    , beginy_(by), originy_(oy), endy_(ey)
    { }
};

// Note. The convolution operation is defined by
//    (f*g)(x) = \int f(x-y) g(y) dy,
// i.e. one operand is reflected before the integration is performed.
// If you don't want this to happen, the behaviour you want is not
// called "convolution". So don't break the convolution routines in
// that particular way.

//: Convolution in x-direction : out(x, y) = \sum_i kernel[i]*in(x-i, y)
template <class I1, class I2, class AC, class O>
void vil1_convolve_1d_x(vil1_convolve_signal_1d<I1 const> const &kernel,
                        vil1_convolve_signal_2d<I2 const> const &input,
                        AC * /*accumulator type*/,
                        vil1_convolve_signal_2d<O> const &output,
                        vil1_convolve_boundary_option b,
                        vil1_convolve_boundary_option e);

//: Convolution in y-direction : out(x, y) = \sum_j kernel[j]*in(x, y-j)
template <class I1, class I2, class AC, class O>
void vil1_convolve_1d_y(vil1_convolve_signal_1d<I1 const> const &kernel,
                        vil1_convolve_signal_2d<I2 const> const &input,
                        AC * /*accumulator type*/,
                        vil1_convolve_signal_2d<O> const &output,
                        vil1_convolve_boundary_option b,
                        vil1_convolve_boundary_option e);

template <class I1, class I2, class AC, class O>
void vil1_convolve_separable(I1 const kernel[], unsigned N,
                             vil1_memory_image_of<I2>& buf,
                             vil1_memory_image_of<AC>& tmp,
                             vil1_memory_image_of<O>& out);

template <class I1, class I2, class AC, class O>
vil1_image vil1_convolve_separable(vil1_image const& in,
                                   I1 const* kernel,
                                   int N, I2*, AC*, O*);

// *** the following function is implemented in vil1_convolve_1d_x.txx

//: Convolution in x-direction, using a symmetric kernel.
template <class I1, class I2, class AC, class O>
void vil1_convolve_1d_x(I1 const *half_kernel, unsigned kernel_size,
                        vil1_convolve_signal_2d<I2 const> const &input,
                        AC * /*accumulator type*/,
                        vil1_convolve_signal_2d<O> const &output,
                        vil1_convolve_boundary_option b,
                        vil1_convolve_boundary_option e);

// *** the following function is implemented in vil1_convolve_1d_y.txx

//: Convolution in y-direction, using a symmetric kernel.
template <class I1, class I2, class AC, class O>
void vil1_convolve_1d_y(I1 const *half_kernel, unsigned kernel_size,
                        vil1_convolve_signal_2d<I2 const> const &input,
                        AC * /*accumulator type*/,
                        vil1_convolve_signal_2d<O> const &output,
                        vil1_convolve_boundary_option b,
                        vil1_convolve_boundary_option e);

#endif // vil1_convolve_h_
