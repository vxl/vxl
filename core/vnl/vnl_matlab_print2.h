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
//   cerr << "foo = " << matlab(foo) << endl;

#include <vnl/vnl_matlab_print.h>

template <class T> 
struct vnl_mprox {
  T const &obj;
  vnl_mprox(T const &obj_) : obj(obj_) { }
  ~vnl_mprox() { }
};

template <class T> 
inline
ostream &operator<<(ostream &os, vnl_mprox<T> const &mp) {
  os << "[";
  vnl_matlab_print(os, mp.obj, 0);
  os << "]";
  return os;
}

template <class T> 
inline
vnl_mprox<T> matlab(T const &obj) { 
  return vnl_mprox<T>(obj);
}

#endif
