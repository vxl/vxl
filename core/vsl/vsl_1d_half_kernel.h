#ifndef vsl_1d_half_kernel_h_
#define vsl_1d_half_kernel_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

//: This structure holds half of a symmetric convolution kernel.
// The centre of the kernel is array[0].
template <class T>
struct vsl_1d_half_kernel {
  vsl_1d_half_kernel(unsigned capacity_ =40)
    : capacity(capacity_)
    , count(0)
    , array(new T[capacity]) { }
  
  ~vsl_1d_half_kernel() { delete [] array; array = 0; }
  
  unsigned const capacity;
  
  unsigned count;
  T *array;
};

template <class T>
void vsl_create_gaussian (T gauss_sigma, vsl_1d_half_kernel<T> *mask_ptr);

#endif
