// This is oxl/osl/osl_1d_half_kernel.h
#ifndef osl_1d_half_kernel_h_
#define osl_1d_half_kernel_h_
//:
//  \file
// \author fsm

//: This structure holds half of a symmetric convolution kernel.
// The centre of the kernel is array[0].
template <class T>
struct osl_1d_half_kernel
{
  osl_1d_half_kernel(unsigned capacity_ =40)
    : capacity(capacity_)
    , count(0)
    , array(new T[capacity]) { }

  ~osl_1d_half_kernel() { delete [] array; array = nullptr; }

  unsigned const capacity;

  unsigned count;
  T *array;
};

template <class T>
void osl_create_gaussian (T gauss_sigma, osl_1d_half_kernel<T> *mask_ptr);

#endif // osl_1d_half_kernel_h_
