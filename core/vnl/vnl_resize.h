#ifndef vnl_resize_h_
#define vnl_resize_h_
// This is vxl/vnl/vnl_resize.h

//: \file
// \brief
// \author fsm@robots.ox.ac.uk

// Modifications
// 4/4/01 LSb (Manchester) Documentation tidied

template <class T> class vnl_vector;
template <class T> class vnl_matrix;
template <class T> class vnl_diag_matrix;

template <class T> void vnl_resize(vnl_vector<T> &v, unsigned newsize);
template <class T> void vnl_resize(vnl_matrix<T> &M, unsigned newrows, unsigned newcols);
template <class T> void vnl_resize(vnl_diag_matrix<T> &D, unsigned newsize);

#endif // vnl_resize_h_
