/*
  fsm@robots.ox.ac.uk
*/
#include "vnl_resize.h"

//static void * operator new(unsigned,void *p) { return p; }
#include <vcl/vcl_new.h>

// fsm: What was wrong with obj.~T()? It's defined in the 
// language, whereas destroy() is defined in a library header.
#if defined(VCL_SUNPRO_CC_50) || defined(VCL_WIN32)
// SunPro <algorithm> does not supply destroy() - sigh...
// vc60 supplied allocator<T>::destroy(T *) - groan...
template <class T> inline void destroy(T *p) { p->~T(); }
#else
# include <vcl/vcl_algorithm.h> // for destroy
#endif

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

//--------------------------------------------------------------------------------

template <class T>
void vnl_resize(vnl_vector<T> &v, unsigned newsize) {
  // copy
  vnl_vector<T> old_v(v);
  
  // destruct
  destroy(&v);
  
  // construct
  new (&v) vnl_vector<T>(newsize);
  
  //
  for (unsigned i=0; i<v.size() && i<old_v.size(); ++i)
    v[i] = old_v[i];
}

template <class T>
void vnl_resize(vnl_matrix<T> &M, unsigned newrows, unsigned newcols) {
  // copy
  vnl_matrix<T> old_M(M);
  
  // destruct
  destroy(&M);
  
  // construct
  new (&M) vnl_matrix<T>(newrows,newcols);

  //  
  for (unsigned i=0; i<M.rows() && i<old_M.rows(); ++i)
    for (unsigned j=0; j<M.cols() && j<old_M.cols(); ++j)
      M(i,j) = old_M(i,j);
}

//--------------------------------------------------------------------------------

template <class T>
void vnl_assign(vnl_vector<T> &lhs, vnl_vector<T> const &rhs) {
  if (&lhs == &rhs)
    return;
  if (lhs.size() == rhs.size()) {
    lhs = rhs;
    return;
  }
  destroy(&lhs);
  new (&lhs) vnl_vector<T>(rhs); // construct
}

template <class T>
void vnl_assign(vnl_matrix<T> &lhs, vnl_matrix<T> const &rhs) {
  if (&lhs == &rhs)
    return;
  if (lhs.rows() == rhs.rows() && lhs.cols() == rhs.cols()) {
    lhs = rhs;
    return;
  }
  destroy(&lhs);
  new (&lhs) vnl_matrix<T>(rhs); // construct
}

//--------------------------------------------------------------------------------

#define VNL_RESIZE_INSTANTIATE(T) \
template void vnl_resize(vnl_vector<T > &, unsigned); \
template void vnl_resize(vnl_matrix<T > &, unsigned, unsigned); \
template void vnl_assign(vnl_vector<T > &, vnl_vector<T > const &); \
template void vnl_assign(vnl_matrix<T > &, vnl_matrix<T > const &);

VNL_RESIZE_INSTANTIATE(float);
VNL_RESIZE_INSTANTIATE(double);
