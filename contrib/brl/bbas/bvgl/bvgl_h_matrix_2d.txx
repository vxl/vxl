// This is brl/bbas/bvgl/bvgl_h_matrix_2d.txx
#ifndef bvgl_h_matrix_2d_txx_
#define bvgl_h_matrix_2d_txx_
//:
// \file

#include "bvgl_h_matrix_2d.h"
#include <vnl/vnl_inverse.h>
#include <vcl_fstream.h>

//--------------------------------------------------------------
//

//: Default constructor
template <class T>
bvgl_h_matrix_2d<T>::bvgl_h_matrix_2d()
{
}

//: Copy constructor
template <class T>
bvgl_h_matrix_2d<T>::bvgl_h_matrix_2d(const bvgl_h_matrix_2d<T>& M)
{
  t12_matrix_ = M.t12_matrix_;
}


//: Constructor from vcl_istream
template <class T>
bvgl_h_matrix_2d<T>::bvgl_h_matrix_2d(vcl_istream& s)
{
  t12_matrix_.read_ascii(s);
}

//: Constructor from file
template <class T>
bvgl_h_matrix_2d<T>::bvgl_h_matrix_2d(char const* filename)
{
  vcl_ifstream f(filename);
  if (!f.good())
    vcl_cerr << "bvgl_h_matrix_2d::read: Error opening " << filename << vcl_endl;
  else
    t12_matrix_.read_ascii(f);
}

//--------------------------------------------------------------
//: Constructor
template <class T>
bvgl_h_matrix_2d<T>::bvgl_h_matrix_2d(vnl_matrix_fixed<T,3,3> const& M):
  t12_matrix_(M)
{
}

//--------------------------------------------------------------
//: Constructor
template <class T>
bvgl_h_matrix_2d<T>::bvgl_h_matrix_2d(const T* H)
  : t12_matrix_(H)
{
}

//: Destructor
template <class T>
bvgl_h_matrix_2d<T>::~bvgl_h_matrix_2d()
{
}

// == OPERATIONS ==

template <class T>
vgl_homg_point_2d<T>
bvgl_h_matrix_2d<T>::operator()(vgl_homg_point_2d<T> const& p) const
{
  vnl_vector_fixed<T, 3> v(p.x(), p.y(), p.w());
  vnl_vector_fixed<T,3> v2 = t12_matrix_ * v;
  return vgl_homg_point_2d<T>(v2.x(), v2.y(), v2.z());
}

template <class T>
vgl_homg_line_2d<T>
bvgl_h_matrix_2d<T>::preimage(vgl_homg_line_2d<T> const& l) const
{
  vnl_vector_fixed<T,3> v(l.a(), l.b(), l.c());
  vnl_vector_fixed<T,3> v2 = t12_matrix_.transpose() * v;
  return vgl_homg_line_2d<T>(v2.x(), v2.y(), v2.z());
}

template <class T>
vgl_homg_line_2d<T>
bvgl_h_matrix_2d<T>::correlation(vgl_homg_point_2d<T> const& p) const
{
  vnl_vector_fixed<T, 3> v(p.x(), p.y(), p.w());
  vnl_vector_fixed<T,3> v2 = t12_matrix_ * v;
  return vgl_homg_line_2d<T>(v2.x(), v2.y(), v2.z());
}

template <class T>
vgl_homg_point_2d<T>
bvgl_h_matrix_2d<T>::correlation(vgl_homg_line_2d<T> const& l) const
{
  vnl_vector_fixed<T,3> v(l.a(), l.b(), l.c());
  vnl_vector_fixed<T,3> v2 = t12_matrix_ * v;
  return vgl_homg_point_2d<T>(v2.x(), v2.y(), v2.z());
}

template <class T>
vgl_conic<T> bvgl_h_matrix_2d<T>::operator() (vgl_conic<T> const& C) const
{
  T a=C.a(), b=C.b()/2, c = C.c(), d = C.d()/2, e = C.e()/2, f = C.f();
  vnl_matrix_fixed<T, 3, 3> M, Mp;
  M.put(0,0,a);  M.put(0,1,b); M.put(0,2,d);
  M.put(1,0,b);  M.put(1,1,c); M.put(1,2,e);
  M.put(2,0,d);  M.put(2,1,e); M.put(2,2,f);
  Mp = t12_matrix_.transpose()*M*t12_matrix_;
  return   vgl_conic<T>(Mp(0,0),(Mp(0,1)+Mp(1,0)),Mp(1,1),(Mp(0,2)+Mp(2,0)),
                        (Mp(1,2)+Mp(2,1)), Mp(2,2));
}
template <class T>
vgl_homg_point_2d<T>
bvgl_h_matrix_2d<T>::preimage(vgl_homg_point_2d<T> const& p) const
{
  vnl_vector_fixed<T,3> v(p.x(), p.y(), p.w());
  vnl_vector_fixed<T,3> v2 = vnl_inverse(t12_matrix_) * v;
  return vgl_homg_point_2d<T>(v2.x(), v2.y(), v2.z());
}

template <class T>
vgl_homg_line_2d<T>
bvgl_h_matrix_2d<T>::operator()(vgl_homg_line_2d<T> const& l) const
{
  vnl_vector_fixed<T,3> v(l.a(), l.b(), l.c());
  vnl_vector_fixed<T,3> v2 = vnl_inverse(t12_matrix_).transpose() * v;
  return vgl_homg_line_2d<T>(v2.x(), v2.y(), v2.z());
}

//-----------------------------------------------------------------------------
//: Print H on vcl_ostream
template <class T>
vcl_ostream& operator<<(vcl_ostream& s, const bvgl_h_matrix_2d<T>& h)
{
  return s << h.get_matrix();
}

//: Read H from vcl_istream
template <class T>
vcl_istream& operator >> (vcl_istream& s, bvgl_h_matrix_2d<T>& H)
{
  H.read(s);
  return s;
}

//: Read H from vcl_istream
template <class T>
bool bvgl_h_matrix_2d<T>::read(vcl_istream& s)
{
  return t12_matrix_.read_ascii(s);
}

//: Read H from file
template <class T>
bool bvgl_h_matrix_2d<T>::read(char const* filename)
{
  vcl_ifstream f(filename);
  if (!f.good())
    vcl_cerr << "bvgl_h_matrix_2d::read: Error opening " << filename << vcl_endl;
  return read(f);
}

// == DATA ACCESS ==

//-----------------------------------------------------------------------------
//: Get matrix element at (row_index, col_index)
template <class T>
T bvgl_h_matrix_2d<T>::get(unsigned int row_index, unsigned int col_index) const
{
  return t12_matrix_. get(row_index, col_index);
}

//: Fill H with contents of this
template <class T>
void bvgl_h_matrix_2d<T>::get(T* H) const
{
  for (int row_index = 0; row_index < 3; row_index++)
    for (int col_index = 0; col_index < 3; col_index++)
      *H++ = t12_matrix_.get(row_index, col_index);
}

//: Fill H with contents of this
template <class T>
void bvgl_h_matrix_2d<T>::get(vnl_matrix<T>* H) const
{
  *H = t12_matrix_;
}

//: Set to identity
template <class T>
void bvgl_h_matrix_2d<T>::set_identity()
{
  t12_matrix_.set_identity();
}

//: Set to 3x3 row-stored matrix
template <class T>
void bvgl_h_matrix_2d<T>::set(const T* H)
{
  for (int row_index = 0; row_index < 3; row_index++)
    for (int col_index = 0; col_index < 3; col_index++)
      t12_matrix_.put(row_index, col_index, *H++);
}

//: Set to given vnl_matrix
template <class T>
void bvgl_h_matrix_2d<T>::set(vnl_matrix_fixed<T,3,3> const& H)
{
  t12_matrix_ = H;
}

//: Return the inverse
template <class T>
const bvgl_h_matrix_2d<T> bvgl_h_matrix_2d<T>::get_inverse() const
{
  vnl_matrix_fixed<T, 3, 3> temp = vnl_inverse(t12_matrix_);
  return bvgl_h_matrix_2d<T>(temp);
}


//----------------------------------------------------------------------------
#undef BVGL_H_MATRIX_2D_INSTANTIATE
#define BVGL_H_MATRIX_2D_INSTANTIATE(T) \
template class bvgl_h_matrix_2d<T >; \
template vcl_ostream& operator << (vcl_ostream& s, const bvgl_h_matrix_2d<T >& h); \
template vcl_istream& operator >> (vcl_istream& s, bvgl_h_matrix_2d<T >& h)

#endif // bvgl_h_matrix_2d_txx_
