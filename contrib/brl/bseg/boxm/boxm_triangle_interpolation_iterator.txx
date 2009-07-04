#ifndef boxm_triangle_interpolation_iterator_txx_
#define boxm_triangle_interpolation_iterator_txx_
//:
// \file

#include "boxm_triangle_interpolation_iterator.h"

#include <vnl/algo/vnl_determinant.h>
#include <vcl_vector.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>


//: constructor
template<class T>
boxm_triangle_interpolation_iterator<T>::boxm_triangle_interpolation_iterator(double *verts_x, double *verts_y, T *vals, unsigned int v0, unsigned int v1, unsigned int v2)
: tri_it_()
{
  tri_it_.a.x = verts_x[v0]  - 0.5;
  tri_it_.a.y = verts_y[v0]  - 0.5;
  tri_it_.b.x = verts_x[v1]  - 0.5;
  tri_it_.b.y = verts_y[v1]  - 0.5;
  tri_it_.c.x = verts_x[v2]  - 0.5;
  tri_it_.c.y = verts_y[v2]  - 0.5;


  // compute s0, s1, s2 such that  val = s0*x + s1*y + s2 for any point within the triangle
  double Acol0[] = {tri_it_.a.x + 0.5, tri_it_.b.x + 0.5, tri_it_.c.x + 0.5};
  double Acol1[] = {tri_it_.a.y + 0.5, tri_it_.b.y + 0.5, tri_it_.c.y + 0.5};
  double Acol2[] = {1.0, 1.0, 1.0};

  double Z[] = {vals[v0], vals[v1], vals[v2]};

  double detA = vnl_determinant(Acol0, Acol1, Acol2);
  s0_ = vnl_determinant(Z, Acol1, Acol2) / detA;
  s1_ = vnl_determinant(Acol0, Z, Acol2) / detA;
  s2_ = vnl_determinant(Acol0, Acol1, Z) / detA;

  //vcl_cout << "s0 = " << s0_ <<"  s1 = " << s1_ << " s2 = " << s2_ << vcl_endl;
}


//: Resets the scan iterator to before the first scan line
//  After calling this function, next() needs to be called before
//  startx() and endx() form a valid scan line.
template<class T>
void boxm_triangle_interpolation_iterator<T>::reset()
{
  tri_it_.reset();
}

//: Tries to move to the next scan line.
//  Returns false if there are no more scan lines.
template<class T>
bool boxm_triangle_interpolation_iterator<T>::next()
{
  return tri_it_.next();
}

//: y-coordinate of the current scan line.
template<class T>
int boxm_triangle_interpolation_iterator<T>::scany() const
{
  return tri_it_.scany();
}

//: Returns starting x-value of the current scan line.
//  startx() should be smaller than endx(), unless the scan line is empty
template<class T>
int boxm_triangle_interpolation_iterator<T>::startx() const
{
  return tri_it_.startx();
}

//: Returns ending x-value of the current scan line.
//  endx() should be larger than startx(), unless the scan line is empty
template<class T>
int  boxm_triangle_interpolation_iterator<T>::endx() const
{
  return tri_it_.endx() + 1;
}


//: returns the interpolated value at location x in the current scanline
template<class T>
T boxm_triangle_interpolation_iterator<T>::value_at(int x)
{
  return (T)((s0_*(x+0.5) + s1_*(tri_it_.scany()+0.5) + s2_));
}


#define BOXM_TRIANGLE_INTERPOLATION_ITERATOR_INSTANTIATE(T) \
  template class boxm_triangle_interpolation_iterator<T >


#endif
