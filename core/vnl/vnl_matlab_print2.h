#ifndef vnl_matlab_print2_h_
#define vnl_matlab_print2_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/
// After including this header file, the client should be able to say :
//   vnl_matrix<double> foo(3, 14);
// ....
//   cerr << "and the blasted matrix is :" << endl
//        << vnl_matlab_print(foo)
//        << vnl_matlab_print(foo, "foo")
//        << vnl_matlab_print(foo, 0, vnl_matlab_fmt_long);
// instead of
// ....
//   cerr << "and the blasted matrix is :" << endl;
//        vnl_matlab_print(cerr, foo);
//        vnl_matlab_print(cerr, foo, "foo");
//        vnl_matlab_print(cerr, foo, 0, vnl_matlab_fmt_long);

#include <vnl/vnl_matlab_print.h>

// The proxy classes.
template <class T> 
struct vnl_matlab_print_proxy 
{
  T const &obj;
  char const *name;
  vnl_matlab_Format format;
  vnl_matlab_print_proxy(T const &obj_,
			 char const *name_, 
			 vnl_matlab_Format format_)
    : obj(obj_), name(name_), format(format_) { }
  ~vnl_matlab_print_proxy() { }
};

// Output operator for the proxies.
template <class T> 
inline
ostream &operator<<(ostream &os, vnl_matlab_print_proxy<T> const &mp) 
{
  return vnl_matlab_print(os, mp.obj, mp.name, mp.format);
}

// Functions to make proxies. This should work for objects of types
// derived from vnl_vector, vnl_matrix etc because the overload
// resolution is done in the operator<< above.
template <class T> 
inline
vnl_matlab_print_proxy<T>
vnl_matlab_print(T const &obj, 
		 char const *name = 0, 
		 vnl_matlab_Format format = vnl_matlab_fmt_default)
{
  return vnl_matlab_print_proxy<T>(obj, name, format);
}

#define VNL_MATLAB_PRINT2_INSTANTIATE(T) \
template struct vnl_matlab_print_proxy<T >; \
VCL_INSTANTIATE_INLINE(ostream &operator<<(ostream &, \
                                           vnl_matlab_print_proxy<T > const &)); \
VCL_INSTANTIATE_INLINE(vnl_matlab_print_proxy<T > vnl_matlab_print(T const &, \
                                                                   char const *, \
                                                                   vnl_matlab_Format));

#endif
