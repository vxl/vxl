#ifndef vnl_determinant_h_
#define vnl_determinant_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

template <class T>
T vnl_determinant(T const * const *rows, unsigned n);

template <class T> class vnl_matrix;

template <class T>
T vnl_determinant(vnl_matrix<T> const &);

#endif
