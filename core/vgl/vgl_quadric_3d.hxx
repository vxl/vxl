// This is core/vgl/vgl_quadric_3d.hxx
#ifndef vgl_quadric_3d_hxx_
#define vgl_quadric_3d_hxx_
#include "vgl_quadric_3d.h"
#include "vgl_tolerance.h"
#include <iostream>
#include <algorithm>
#include <functional>

static const char* quadric_class_name[] = 
{
  "invalid_quadric",
    "coincident_planes",   
    "imaginary_ellipsoid",
    "real_ellipsoid",
    "imaginary_elliptic_cone",
    "real_elliptic_cone",
    "imaginary_elliptic_cylinder",
    "real_elliptic_cylinder",
    "elliptic_paraboloid",
    "hyperbolic_cylinder",
    "hyperbolic_paraboloid",
    "hyperboloid_of_one_sheet",
    "hyperboloid_of_two_sheets",
    "imaginary_intersecting_planes",
    "real_intersecting_planes",
    "parabolic_cylinder",
    "imaginary_parallel_planes",
    "real_parallel planes"
};
template <class T>
std::string vgl_quadric_3d<T>::type_by_number(vgl_quadric_type const& type) { return quadric_class_name[(size_t)type]; }

template <class T>
typename vgl_quadric_3d<T>::vgl_quadric_type vgl_quadric_3d<T>::type_by_name(std::string const& name){
    for(size_t i = 1; i<num_quadric_types; i++){
      if(quadric_class_name[i]==name)
        return static_cast<vgl_quadric_type>(i);
    }
    return no_type;
}

template <class T>
vgl_quadric_3d<T>::vgl_quadric_3d(T a, T b, T c, T d, T e, T f, T g, T h, T i, T j):det_zero_(false){
  this->set(a, b, c, d,  e, f, g, h, i, j);
}
template <class T>
vgl_quadric_3d<T>::vgl_quadric_3d(T const coeff[]):det_zero_(false){
  this->set(coeff[0],coeff[1],coeff[2],coeff[3],coeff[4],coeff[5],coeff[6],coeff[7],coeff[8],coeff[9]);
}
  //       _                  _
  //      |  a   d/2  e/2  g/2 |   
  //      | d/2   b   f/2  h/2 |  
  // Q =  | e/2  f/2   c   i/2 |
  //      | g/2  h/2  i/2   j  |
  //       -                  -
  //
template <class T>
vgl_quadric_3d<T>::vgl_quadric_3d(std::vector<std::vector<T> > const& Q):det_zero_(false){
  this->set(Q[0][0], Q[1][1], Q[2][2], T(2)*Q[0][1], T(2)*Q[0][2],
    T(2)*Q[1][2], T(2)*Q[0][3], T(2)*Q[1][3], T(2)*Q[2][3], Q[3][3]);
}

template <class T>
void vgl_quadric_3d<T>::set(T a, T b, T c, T d, T e, T f, T g, T h, T i, T j){
  a_ = a; b_ = b; c_ = c; d_ = d; e_ = e; f_ = f;
  g_ = g; h_ = h; i_ = i; j_ = j;
  this->compute_type();
}
template <class T>
void vgl_quadric_3d<T>::set(std::vector<std::vector<T> > const& Q){
  this->set(Q[0][0], Q[1][1], Q[2][2],  T(2)*Q[0][1],
            T(2)*Q[0][2], T(2)*Q[1][2], T(2)*Q[0][3],
            T(2)*Q[1][3], T(2)*Q[2][3], Q[3][3]);
}
template <class T>
std::vector<std::vector<T> > vgl_quadric_3d<T>::coef_matrix() const{
  vcl_vector<std::vector<T> > Q(4,vcl_vector<T>(4,T(0)));
  Q[0][0]=a_;Q[1][1]=b_; Q[2][2]=c_; Q[3][3]=j_;
  Q[0][1]= Q[1][0]=d_/T(2); Q[0][2]= Q[2][0]=e_/T(2);
  Q[0][3]= Q[3][0]=g_/T(2); Q[1][2]= Q[2][1]=f_/T(2);
  Q[1][3]= Q[3][1]=h_/T(2); Q[3][2]=Q[2][3] =i_/T(2); 
  return Q;
}
template <class T>
bool vgl_quadric_3d<T>::on(vgl_homg_point_3d<T> const& pt, T tol) const{
  T x = pt.x(), y = pt.y(), z = pt.z();
  T algebraic_dist = a_*x*x + b_*y*y + c_*z*z + d_*x*y + e_*x*z + f_*y*z + g_*x + h_*y + i_*z +j_;
  T grad_x = (T(2)*a_*x + d_*y * e_*z + g_);
  T grad_y = (T(2)*b_*y + d_*x * f_*z + h_);
  T grad_z = (T(2)*c_*z + e_*x * f_*y + i_ );
  T grad_mag_sqrd = grad_x*grad_x + grad_y*grad_y + grad_z*grad_z;
  T sampson_dist_sqrd = (algebraic_dist*algebraic_dist)/grad_mag_sqrd;
  if(sqrt(sampson_dist_sqrd) < tol)
    return true;
  return false;
}
template <class T>
void vgl_quadric_3d<T>::compute_type(){
  type_ = no_type;
  std::vector<std::vector<T> > Q = this->coef_matrix();
  T m[4][4]; T l[4]; T vc[4][4];
  for(size_t r = 0; r<4; ++r)
    for(size_t c = 0; c<4; ++c)
      m[r][c] = Q[r][c];

  T mu[3][3]; T lu[3]; T vcu[3][3];
  for(size_t r = 0; r<3; ++r)
    for(size_t c = 0; c<3; ++c)
      mu[r][c] = Q[r][c];

  // the vector l contains the eigenvalues of the coeficient matrix
  eigen<T, 4>( m, l, vc);
std::vector<T> lv;
  //determine the rank of Q
  T rank_tol = T(1) / T(1000);
  std::vector<T> eig_vals;
  for(size_t i =0; i<4; ++i){
    eig_vals.push_back(fabs(l[i]));
	lv.push_back(l[i]);
}

  std::sort(eig_vals.begin(), eig_vals.end(), std::greater<T>());
  T largest_eig_val = eig_vals[0];
  size_t r4 = 0;
  for (size_t i = 0; i < 4; ++i) {
    eig_vals[i] /= largest_eig_val;
    if (eig_vals[i] > rank_tol)
      r4++;
  }
  std::vector<T> non_zero_vals;
  for(size_t i = 0 ; i<4; ++i){
    if(fabs(lv[i])/largest_eig_val <= rank_tol)
       continue;
    non_zero_vals.push_back(lv[i]);
  }
  T z = T(0);
  size_t nz = non_zero_vals.size();
  if(nz == 0)
    return;
  bool sign = true;
  if(nz == 2)
    sign =(non_zero_vals[0]>z && non_zero_vals[1]>z)||
      (non_zero_vals[0]<z && non_zero_vals[1]<z);
  else if(nz == 3)
    sign = (non_zero_vals[0]>z && non_zero_vals[1]>z && non_zero_vals[2]>z) ||
      (non_zero_vals[0]<z&& non_zero_vals[1]< z&& non_zero_vals[2]< z);
  else if(nz == 4)
    sign=(non_zero_vals[0]>z&&non_zero_vals[1]>z&&non_zero_vals[2]>z&&non_zero_vals[3]>z)||
      (non_zero_vals[0]<z&& non_zero_vals[1]< z&& non_zero_vals[2]< z&& non_zero_vals[3]< z);


  //determine the rank of upper 3x3 of Q
  std::vector<T> lvu;
  eigen<T, 3>( mu, lu, vcu);
  std::vector<T> upper_eig_vals;
  for(size_t i =0; i<3; ++i){
   upper_eig_vals.push_back(fabs(lu[i]));
   lvu.push_back(lu[i]);
 }
  std::sort(upper_eig_vals.begin(), upper_eig_vals.end(), std::greater<T>());
  largest_eig_val = upper_eig_vals[0];
  size_t r3 = 0;
  for (size_t i = 0; i < 3; ++i) {
    upper_eig_vals[i] /= largest_eig_val;
    if (upper_eig_vals[i] > rank_tol){
      r3++;
    }
  }
  std::vector<T> non_zero_uvals;
  for(size_t i = 0 ; i<3; ++i){
    if(fabs(lvu[i])/largest_eig_val <= rank_tol)
       continue;
    non_zero_uvals.push_back(lvu[i]);
  }
  nz = non_zero_uvals.size();
  if(nz == 0)
    return;
  bool signu = true;
  if(nz == 2)
    signu =(non_zero_uvals[0]>z && non_zero_uvals[1]>z)||
      (non_zero_uvals[0]<z && non_zero_uvals[1]<z);
  else if(nz == 3)
    signu = (non_zero_uvals[0]>z && non_zero_uvals[1]>z && non_zero_uvals[2]>z) ||
      (non_zero_uvals[0]<z&& non_zero_uvals[1]< z&& non_zero_uvals[2]< z);

  //compute the determinant of Q
T det =  Q[0][0]*Q[1][1]*Q[2][2]*Q[3][3]
    - Q[0][0]*Q[1][1]*Q[3][2]*Q[2][3]
    - Q[0][0]*Q[2][1]*Q[1][2]*Q[3][3]
    + Q[0][0]*Q[2][1]*Q[3][2]*Q[1][3]
    + Q[0][0]*Q[3][1]*Q[1][2]*Q[2][3]
    - Q[0][0]*Q[3][1]*Q[2][2]*Q[1][3]
    - Q[1][0]*Q[0][1]*Q[2][2]*Q[3][3]
    + Q[1][0]*Q[0][1]*Q[3][2]*Q[2][3]
    + Q[1][0]*Q[2][1]*Q[0][2]*Q[3][3]
    - Q[1][0]*Q[2][1]*Q[3][2]*Q[0][3]
    - Q[1][0]*Q[3][1]*Q[0][2]*Q[2][3]
    + Q[1][0]*Q[3][1]*Q[2][2]*Q[0][3]
    + Q[2][0]*Q[0][1]*Q[1][2]*Q[3][3]
    - Q[2][0]*Q[0][1]*Q[3][2]*Q[1][3]
    - Q[2][0]*Q[1][1]*Q[0][2]*Q[3][3]
    + Q[2][0]*Q[1][1]*Q[3][2]*Q[0][3]
    + Q[2][0]*Q[3][1]*Q[0][2]*Q[1][3]
    - Q[2][0]*Q[3][1]*Q[1][2]*Q[0][3]
    - Q[3][0]*Q[0][1]*Q[1][2]*Q[2][3]
    + Q[3][0]*Q[0][1]*Q[2][2]*Q[1][3]
    + Q[3][0]*Q[1][1]*Q[0][2]*Q[2][3]
    - Q[3][0]*Q[1][1]*Q[2][2]*Q[0][3]
    - Q[3][0]*Q[2][1]*Q[0][2]*Q[1][3]
    + Q[3][0]*Q[2][1]*Q[1][2]*Q[0][3];
 
 T tol = vgl_tolerance<T>::position;
 if(fabs(det)<T(100)*tol)
   det_zero_ = true;
 bool gt_0 = det>z;
/*
 quadric type                          equation                      r3  r4  gt_0    signu sign
coincident_planes              x^2=0                                  1  1
imaginary_ellipsoid           (x^2)/(a^2)+(y^2)/(b^2)+(z^2)/(c^2)=-1  3  4   true    true
real_ellipsoid                (x^2)/(a^2)+(y^2)/(b^2)+(z^2)/(c^2)=1   3  4   false   true
imaginary_elliptic_cone       (x^2)/(a^2)+(y^2)/(b^2)+(z^2)/(c^2)=0   3  3           true
real_elliptic_cone            (x^2)/(a^2)+(y^2)/(b^2)-(z^2)/(c^2)=0   3  3           false
imaginary_elliptic_cylinder   (x^2)/(a^2)+(y^2)/(b^2)=-1              2  3           true  true
real_elliptic_cylinder        (x^2)/(a^2)+(y^2)/(b^2)=1               2  3           true  false 
elliptic_paraboloid           z=(x^2)/(a^2)+(y^2)/(b^2)               2  4   false   true 
hyperbolic_cylinder           (x^2)/(a^2)-(y^2)/(b^2)=-1              2  3           false
hyperbolic_paraboloid         z=(y^2)/(b^2)-(x^2)/(a^2)               2  4   true    false
hyperboloid_of_one_sheet      (x^2)/(a^2)+(y^2)/(b^2)-(z^2)/(c^2)=1   3  4   true    false
hyperboloid_of_two_sheets     (x^2)/(a^2)+(y^2)/(b^2)-(z^2)/(c^2)=-1  3  4   false   flase
imaginary_intersecting_planes (x^2)/(a^2)+(y^2)/(b^2)=0               2  2           true 
real_intersecting_planes      (x^2)/(a^2)-(y^2)/(b^2)=0               2  2           false
parabolic_cylinder             x^2+2rz=0                              1  3
imaginary_parallel_planes      x^2=-a^2                               1  2                 true
real_parallel planes           x^2=a^2                                1  2                 false
*/
//  the table above
 if(r3==1&&r4==1)                         type_ = coincident_planes;
 else if(r3== 3 &&r4== 4 &&gt_0&&signu)   type_ = imaginary_ellipsoid;
 else if(r3== 3 &&r4== 4 &&!gt_0&&signu)  type_ = real_ellipsoid;
 else if(r3== 3 &&r4== 3 &&signu)         type_ = imaginary_elliptic_cone;
 else if(r3== 3 &&r4== 3 &&!signu)        type_ = real_elliptic_cone;
 else if(r3== 2 &&r4== 3 &&signu&&sign)   type_ = imaginary_elliptic_cylinder;
 else if(r3== 2 &&r4== 3 &&signu&&!sign)  type_ = real_elliptic_cylinder;
 else if(r3== 2 &&r4== 4 &&!gt_0&&signu)  type_ = elliptic_paraboloid;
 else if(r3== 2 &&r4== 3 &&!signu)        type_ = hyperbolic_cylinder;
 else if(r3== 2 &&r4== 4 &&gt_0&&!signu)  type_ = hyperbolic_paraboloid;
 else if(r3== 3 &&r4== 4 &&gt_0&&!signu)  type_ = hyperboloid_of_one_sheet;
 else if(r3== 3 &&r4== 4 &&!gt_0&&!signu) type_ = hyperboloid_of_two_sheets;
 else if(r3== 2 &&r4== 2 &&signu)         type_ = imaginary_intersecting_planes;
 else if(r3== 2 &&r4== 2 &&!signu)        type_ = real_intersecting_planes;
 else if(r3== 1 &&r4== 3 )                type_ = parabolic_cylinder;
 else if(r3== 1 &&r4== 2 &&sign)          type_ = imaginary_parallel_planes;
 else if(r3== 1 &&r4== 2 &&!sign)         type_ = real_parallel_planes;
}
template <class T>
bool vgl_quadric_3d<T>::operator==(vgl_quadric_3d<T> const& that) const
{
  if ( type() != that.type() ) return false;
  T mag_coefs = (fabs(a_)+fabs(b_)+fabs(c_)+fabs(d_)+fabs(e_)+fabs(f_)+fabs(g_)+fabs(h_)+fabs(i_)+fabs(j_))/T(10);
  T mag_coefs_that = (fabs(that.a())+fabs(that.b())+fabs(that.c())+fabs(that.d())+fabs(that.e())+fabs(that.f())+fabs(that.g())+fabs(that.h())+fabs(that.i())+fabs(that.j()))/T(10);  
  T tol = vgl_tolerance<T>::position;
  return fabs(a_*mag_coefs_that - that.a()*mag_coefs)<tol &&
    fabs(b_*mag_coefs_that - that.b()*mag_coefs)<tol &&
    fabs(c_*mag_coefs_that - that.c()*mag_coefs)<tol &&
    fabs(d_*mag_coefs_that - that.d()*mag_coefs)<tol &&
    fabs(e_*mag_coefs_that - that.e()*mag_coefs)<tol &&
    fabs(f_*mag_coefs_that - that.f()*mag_coefs)<tol &&
    fabs(g_*mag_coefs_that - that.g()*mag_coefs)<tol &&
    fabs(h_*mag_coefs_that - that.h()*mag_coefs)<tol &&
    fabs(i_*mag_coefs_that - that.i()*mag_coefs)<tol &&
    fabs(j_*mag_coefs_that - that.j()*mag_coefs)<tol;
}
// The eigensystem computation below is described at
//http://www.cap-lore.com/MathPhys/eigen/
template <class R, size_t n>
void twst(R m[n][n], R c, R s, int i, int j){
  int k=n;
  while(k--){
    R t = m[i][k]*c+ m[j][k]*s;
    m[j][k] = -s*(m[i][k])+c*(m[j][k]); m[i][k] = t;
  }
}

template <class R, size_t n>
void eigen(R m[n][n], R l[n], R vc[n][n]){
#define db 0
  R m2[n][n];
  {int i=n;
    while(i--) {
      int j=n;
      while(j--) {
        m2[i][j] = m[i][j];
        vc[i][j] = i==j;
      }
    }
  }
  while(1){
    R mod = 0; int i=0, j=0;
    {int k=n; while(k--){
        int m=n;
        while((--m)>k){
          R q = fabs(m2[k][m]);
          if(q > mod) {mod=q; i=k; j=m;
          }
        }
      }
    }
    if(mod < 0.00000000001) break;
    // if(db) printf("mii=%e, mjj=%e\n", m2[i][i], m2[j][j]);
    if(db) std::cout << "mii=" << m2[i][i] << "mjj=" << m2[j][j] << std::endl;
    R th = 0.5*atan(2*m2[i][j]/(m2[i][i] - m2[j][j]));
    // if(db) printf("th=%e, i=%d, j=%d\n", th, i, j);
    if(db) std::cout << "th=" << th << "i" << i << "j=" << j << std::endl;
    {
      R c = cos(th), s = sin(th);
      {int k=n; while(k--){
          R t = c*m2[k][i] + s*m2[k][j];
          m2[k][j] = -s*m2[k][i] + c*m2[k][j]; m2[k][i]=t;
        }
      }
      twst<R,n>( m2, c, s, i, j);
      twst<R,n>( vc, c, s, i, j);
    }
  }
  {
    int j=n;
    while(j--) l[j] = m2[j][j];
  }
}

template <class T>
std::ostream& operator<<(std::ostream& os, const vgl_quadric_3d<T >& q){
  os << "vgl_quadric_3d: a=" << q.a() << " b=" << q.b() << " c=" << q.c() << " d="
     << q.d() << " e=" << q.e()<< " f=" << q.f() << " g=" << q.g()
     << " h=" << q.h() << " i=" << q.i() << " j=" << q.j() << std::endl;
  return os;
}

template <class T>
std::istream& operator>>(std::istream& ist, vgl_quadric_3d<T >& q){
  T a,b,c,d,e,f,g,h,i,j;
  ist >> a >> b >> c >> d >> e >> f >> g >> h >> i >> j;
  q.set(a, b, c, d, e, f, g, h , i, j);
  return ist;}

#undef VGL_QUADRIC_3D_INSTANTIATE
#define VGL_QUADRIC_3D_INSTANTIATE(T) \
template class vgl_quadric_3d<T>;                                    \
template std::ostream& operator<<(std::ostream&, const vgl_quadric_3d<T>&); \
template std::istream& operator>>(std::istream&, vgl_quadric_3d<T>&)
#endif //vgl_quadric
