#include "vgl_cremona_trans_2d.h"
#include <cmath>
static size_t factorial(size_t n){
  size_t ret = 1;  
  for ( size_t i = 2; i <= n; i++ ) {  
    ret = ret * i;  
  }  
  return ret;  
}
// the number of coefficients in a polynomial in X,Y with degree deg
template <class T, size_t deg>
size_t vgl_cremona_trans_2d<T, deg>::n_coeff(){
  return factorial(2+deg)/(2*factorial(deg));
}
// the monomials in a polynomial in X, Y of degree deg 
template <class T, size_t deg>
vnl_vector<T> vgl_cremona_trans_2d<T, deg>::power_vector(T x, T y) {
  size_t nc = vgl_cremona_trans_2d<T, deg>::n_coeff();
  vnl_vector<T> ret(nc, T(0));
  size_t k = 0;
  for(size_t xp = 0; xp<=deg; ++xp)
    for (size_t yp = 0; yp <= (deg - xp); ++yp) {
        ret[k++] = pow(x, xp) * pow(y, yp);
    }
  return ret;
}
template <class T, size_t deg>
void vgl_cremona_trans_2d<T, deg>::project(T X, T Y, T& x, T& y) const{
  vgl_point_2d<T> p(X, Y);
  vgl_point_2d<T> pp = (*this)(p);
  x = pp.x(); y = pp.y();
}
template <class T, size_t deg>
vgl_homg_point_2d<T> vgl_cremona_trans_2d<T, deg>::operator()(vgl_homg_point_2d<T> const& p) const{
    vgl_homg_point_2d<T> norm_hX = tr_from_(p);
  vnl_vector<T> pv = vgl_cremona_trans_2d<T,deg>::power_vector(norm_hX.x(), norm_hX.y());
  size_t nc = vgl_cremona_trans_2d<T, deg>::n_coeff();
  vnl_vector<T> x_neu = coeff_.extract(nc, 0);
  vnl_vector<T> x_den = coeff_.extract(nc, nc);
  vnl_vector<T> y_neu = coeff_.extract(nc, 2*nc);
  vnl_vector<T> y_den = coeff_.extract(nc, 3*nc);
  T px = dot_product(x_neu, pv) / dot_product(x_den, pv);
  T py = dot_product(y_neu, pv) / dot_product(y_den, pv);
  vgl_homg_point_2d<T> hx(px, py);
  return  tr_to_.preimage(hx);
}
template <class T, size_t deg>
vgl_point_2d<T> vgl_cremona_trans_2d<T, deg>::operator()(vgl_point_2d<T> const& p) const{
  vgl_homg_point_2d<T> hp(p);
  return vgl_point_2d<T>((*this)(hp));
}

#undef VGL_CREMONA_TRANS_2D_INSTANTIATE
#define VGL_CREMONA_TRANS_2D_INSTANTIATE(T, deg) \
  template class vgl_cremona_trans_2d<T, deg>
