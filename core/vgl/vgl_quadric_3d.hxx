// This is core/vgl/vgl_quadric_3d.hxx
#ifndef vgl_quadric_3d_hxx_
#define vgl_quadric_3d_hxx_
#include <iostream>
#include <algorithm>
#include <functional>
#include "vgl_quadric_3d.h"
#include "vgl_tolerance.h"
#define RANK_FACTOR 100000
#define DET_FACTOR 100
static const char* quadric_class_name[] =
{
  "invalid_quadric",
  "real_ellipsoid",
  "imaginary_ellipsoid",
  "hyperboloid_of_one_sheet",
  "hyperboloid_of_two_sheets",
  "real_elliptic_cone",
  "imaginary_elliptic_cone",
  "elliptic_paraboloid",
  "hyperbolic_paraboloid",
  "real_elliptic_cylinder",
  "imaginary_elliptic_cylinder",
  "hyperbolic_cylinder",
  "real_intersecting_planes",
  "imaginary_intersecting_planes",
  "parabolic_cylinder",
  "real_parallel_planes",
  "imaginary_parallel_planes",
  "coincident_planes"
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
vgl_quadric_3d<T>::vgl_quadric_3d(std::vector<std::vector<T> > const& canonical_quadric,
                                  std::vector<std::vector<T> > const& H){
  //The 4x4 coefficient matrix, Qg,  in the globa frame is given by
  // Qg =  H^-t Qc H^-1
  // where Qc = canonical_quadric;
  std::vector<std::vector<T> > R(3,std::vector<T>(3, T(0)));
  std::vector<std::vector<T> > Qg(4, std::vector<T>(4, T(0)));
  std::vector<std::vector<T> > qr(3,std::vector<T>(3, T(0)));
  std::vector<T> qu(3,T(0)), t(3, T(0)), c(3,T(0)), Rc(3,T(0)),
    RqRt(3, T(0)), Rtrt(3,T(0));
  for(size_t r = 0; r<3; ++r){
    t[r]=H[r][3];
    qu[r] = canonical_quadric[r][r];
    c[r]=canonical_quadric[r][3];
    for(size_t c = 0; c<3; ++c)
      R[r][c] = H[r][c];
  }
  qr[0][0]=R[0][0]*qu[0]*R[0][0] +R[0][1]*qu[1]*R[0][1] +R[0][2]*qu[2]*R[0][2];
  qr[1][0]=R[1][0]*qu[0]*R[0][0] +R[1][1]*qu[1]*R[0][1] +R[1][2]*qu[2]*R[0][2];
  qr[2][0]=R[2][0]*qu[0]*R[0][0] +R[2][1]*qu[1]*R[0][1] +R[2][2]*qu[2]*R[0][2];
  qr[1][1]=R[1][0]*qu[0]*R[1][0] +R[1][1]*qu[1]*R[1][1] +R[1][2]*qu[2]*R[1][2];
  qr[2][1]=R[2][0]*qu[0]*R[1][0] +R[2][1]*qu[1]*R[1][1] +R[2][2]*qu[2]*R[1][2];
  qr[2][2]=R[2][0]*qu[0]*R[2][0] +R[2][1]*qu[1]*R[2][1] +R[2][2]*qu[2]*R[2][2];
  qr[0][1]=qr[1][0]; qr[0][2]=qr[2][0]; qr[2][1]=qr[1][2];

  Rc[0] = R[0][0]*c[0] + R[0][1]*c[1] + R[0][2]*c[2];
  Rc[1] = R[1][0]*c[0] + R[1][1]*c[1] + R[1][2]*c[2];
  Rc[2] = R[2][0]*c[0] + R[2][1]*c[1] + R[2][2]*c[2];

  Rtrt[0]=R[0][0]*t[0] + R[1][0]*t[1] + R[2][0]*t[2];
  Rtrt[1]=R[0][1]*t[0] + R[1][1]*t[1] + R[2][1]*t[2];
  Rtrt[2]=R[0][2]*t[0] + R[1][2]*t[1] + R[2][2]*t[2];

  RqRt[0] = (qr[0][0]*t[0] + qr[0][1]*t[1] + qr[0][2]*t[2]);
  RqRt[1] = (qr[1][0]*t[0] + qr[1][1]*t[1] + qr[1][2]*t[2]);
  RqRt[2] = (qr[2][0]*t[0] + qr[2][1]*t[1] + qr[2][2]*t[2]);

  T tRqRt = t[0]*RqRt[0] + t[1]*RqRt[1] + t[2]*RqRt[2];
  T tRc = t[0]*Rc[0] + t[1]*Rc[1] + t[2]*Rc[2];
  T ctrRtrt = c[0]*Rtrt[0] + c[1]*Rtrt[1] + c[2]*Rtrt[2];

    Qg[0][3] = Rc[0]-RqRt[0];
  Qg[1][3] = Rc[1]-RqRt[1];
  Qg[2][3] = Rc[2]-RqRt[2];
  Qg[3][0] = Qg[0][3];  Qg[3][1] = Qg[1][3];  Qg[3][2] = Qg[2][3];

  for(size_t r = 0; r<3; ++r)
    Qg[r][r] = qr[r][r];

  Qg[1][0] = qr[1][0];  Qg[2][0] = qr[2][0];  Qg[2][1] = qr[2][1];
  Qg[0][1] = Qg[1][0];  Qg[0][2] = Qg[2][0];  Qg[1][2] = Qg[2][1];
  Qg[3][3] = canonical_quadric[3][3]- ctrRtrt + tRqRt - tRc;
  *this = vgl_quadric_3d<T>(Qg);
}
template <class T>
vgl_quadric_3d<T>::vgl_quadric_3d(std::vector<T> const& diag,
                                  std::vector<std::vector<T> > const& H){
  std::vector<std::vector<T> > Qg(4, std::vector<T>(4, T(0)));
  Qg[0][0] = diag[0];Qg[1][1] = diag[1];Qg[2][2] = diag[2];Qg[3][3] = diag[3];
  *this = vgl_quadric_3d<T>(Qg, H);
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
  std::vector<std::vector<T> > Q(4,std::vector<T>(4,T(0)));
  Q[0][0]=a_;Q[1][1]=b_; Q[2][2]=c_; Q[3][3]=j_;
  Q[0][1]= Q[1][0]=d_/T(2); Q[0][2]= Q[2][0]=e_/T(2);
  Q[0][3]= Q[3][0]=g_/T(2); Q[1][2]= Q[2][1]=f_/T(2);
  Q[1][3]= Q[3][1]=h_/T(2); Q[3][2]=Q[2][3] =i_/T(2);
  return Q;
}
template <class T>
T vgl_quadric_3d<T>::sampson_dist(vgl_homg_point_3d<T> const& pt) const{
  T x = pt.x(), y = pt.y(), z = pt.z(), w = pt.w();
  T algebraic_dist = a_*x*x + b_*y*y + c_*z*z + d_*x*y + e_*x*z + f_*y*z + g_*x*w + h_*y*w + i_*z*w +j_*w*w;
  T grad_x = (T(2)*a_*x + d_*y * e_*z + g_*w);
  T grad_y = (T(2)*b_*y + d_*x * f_*z + h_*w);
  T grad_z = (T(2)*c_*z + e_*x * f_*y + i_*w );
  T grad_mag_sqrd = grad_x*grad_x + grad_y*grad_y + grad_z*grad_z;
  T sampson_dist_sqrd = (algebraic_dist*algebraic_dist)/grad_mag_sqrd;
  return sqrt(sampson_dist_sqrd);
}
template <class T>
bool vgl_quadric_3d<T>::on(vgl_homg_point_3d<T> const& pt, T tol) const{
  T d = this->sampson_dist(pt);
  if(d < tol)
    return true;
  return false;
}
template <class T>
bool vgl_quadric_3d<T>::center(vgl_point_3d<T>& center) const{
  if(!(type_ == real_ellipsoid || type_==real_elliptic_cone ||
       type_ == hyperboloid_of_one_sheet || type_ == hyperboloid_of_two_sheets)
     )
    return false;
  T h = T(1)/T(2);
  //              _            _
  //             |  a   d/2  e/2|
  // upper3x3 =  | d/2   b   f/2|
  //             | e/2  f/2   c |
  //              -            -
  T upper_det = a_*b_*c_ - a_*h*f_*h*f_ - h*d_*h*d_*c_ + d_*h*f_*h*e_*h + e_*h*d_*h*f_*h - e_*h*b_*e_*h;
  if(fabs(upper_det)<T(DET_FACTOR)*vgl_tolerance<T>::position){
    return false;
  }
  T det_inv = T(1)/upper_det;

  T d[9];// inverse of upper 3x3
  d[0] = (b_*c_ -f_*f_*h*h)*det_inv;
  d[1] = (f_*h*e_*h -c_*d_*h)*det_inv;
  d[2] = (d_*h*f_*h -e_*h*b_)*det_inv;
  d[3] = (f_*h*e_*h  -d_*h*c_)*det_inv;
  d[4] = (a_*c_ -e_*h*e_*h)*det_inv;
  d[5] = (d_*h*e_*h -f_*h*a_)*det_inv;
  d[6] = (d_*h*f_*h -b_*e_*h)*det_inv;
  d[7] = (d_*h*e_*h -a_*f_*h)*det_inv;
  d[8] = (a_*b_ -d_*h*d_*h)*det_inv;

  // tx   1  d0  d1  d2   g_
  // ty = -  d3  d4  d5   h_
  // tz   2  d6  d7  d8   i_

  T tx = h*(d[0]*g_ + d[1]*h_ + d[2]*i_);
  T ty = h*(d[3]*g_ + d[4]*h_ + d[5]*i_);
  T tz = h*(d[6]*g_ + d[7]*h_ + d[8]*i_);
  center.set(-tx, -ty, -tz);
  return true;
}
  template <class T>
void vgl_quadric_3d<T>::compute_type(){
  type_ = no_type;
  T tol = vgl_tolerance<T>::position;
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
  T rank_tol = T(RANK_FACTOR)*tol;
  std::vector<T> eig_vals;
  for(auto & i : l){
    eig_vals.push_back(fabs(i));
        lv.push_back(i);
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
  for(auto & i : lu){
   upper_eig_vals.push_back(fabs(i));
   lvu.push_back(i);
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

 if(fabs(det)<T(DET_FACTOR)*tol)
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
template <class T>
void vgl_quadric_3d<T>::upper_3x3_eigensystem(std::vector<T>& eigenvalues, std::vector<std::vector<T> >& eigenvectors) const{
  std::vector<std::vector<T> > Q = this->coef_matrix();
  T mu[3][3]; T lu[3]; T vcu[3][3];
  for(size_t r = 0; r<3; ++r)
    for(size_t c = 0; c<3; ++c)
      mu[r][c] = Q[r][c];

  eigen<T, 3>( mu, lu, vcu);
  for(auto & i : lu){
    eigenvalues.push_back(i);
  }
  eigenvectors.resize(3, std::vector<T>(3, T(0)));
  for(size_t r = 0; r<3; ++r)
    for(size_t c = 0; c<3; ++c)
      eigenvectors[r][c]=vcu[r][c];
}
template <class T>
bool vgl_quadric_3d<T>::canonical_central_quadric(std::vector<T>& diag, std::vector<std::vector<T> >& H) const{
  diag.resize(4, T(0));
  H.resize(4, std::vector<T>(4, T(0)));
  vgl_point_3d<T> cent;
  bool good = this->center(cent);//the quadric origin
  if(!good)
    return false;
  H[0][3] = cent.x(); H[1][3] = cent.y(); H[2][3] = cent.z();
  H[3][3] = T(1);
  // the constant term in the centered coordinate frame
  T centered_j = j_ + (cent.x()*g_ + cent.y()*h_ + cent.z()*i_)/T(2);
  // find the upper 3x3 coordinate system
  std::vector<T> eigenvalues;
  std::vector<std::vector<T> > eigenvectors;
  this->upper_3x3_eigensystem(eigenvalues, eigenvectors);

  for(size_t r = 0; r<3; ++r)
    for(size_t c = 0; c<3; ++c)
      H[r][c]=eigenvectors[c][r];

  for(size_t i = 0; i<3; ++i)
    diag[i]=eigenvalues[i];
  diag[3] = centered_j;
  return true;
}
template <class T>
std::vector<std::vector<T> > vgl_quadric_3d<T>::canonical_quadric(std::vector<std::vector<T> >& H) const{
  std::vector<std::vector<T> > ret(4, std::vector<T>(4, T(0)));
  std::vector<T> tr(3, T(0));
  if(type_ == no_type){
    std::cout << "Invalid quadric" << std::endl;
    return ret;
  }
  //check first for a central quadric
  if(type_ == real_ellipsoid || type_==real_elliptic_cone ||
     type_ == hyperboloid_of_one_sheet || type_ == hyperboloid_of_two_sheets){
    std::vector<T> diag;
    if(canonical_central_quadric(diag, H)){
      for(size_t i = 0; i<4; ++i)
        ret[i][i] = diag[i];
      return ret;
    }else{
      std::cout << "Shouldn't happen! Inconsistent quadric type assignment " << type_by_number(type_) << std::endl;
      return ret;
    }
  }

  H.resize(4, std::vector<T>(4, T(0)));

  // not a central quadric get the eigensystem for the upper 3x3
  std::vector<T> lambda, sorted_eigenvalues(3,T(0));
  std::vector<std::vector<T> > E;
  this->upper_3x3_eigensystem(lambda, E);

  // to rotate the canonical form back to the original frame
  for(size_t r = 0; r<3; ++r)
    for(size_t c = 0; c<3; ++c)
      H[r][c] = E[c][r];
  H[3][3] = T(1);

  for(size_t i = 0; i<3; ++i)
    sorted_eigenvalues[i]=fabs(lambda[i]);
  std::sort(sorted_eigenvalues.begin(), sorted_eigenvalues.end(), std::greater<T>());
  T largest_eigenval = sorted_eigenvalues[0];
  T rtol = T(RANK_FACTOR)*vgl_tolerance<T>::position;
  size_t rank = 3;
  for(size_t i = 0; i<3; ++i)
    if(fabs(lambda[i]/largest_eigenval) < rtol){
      lambda[i]=T(0);
      rank--;
    }
  if(rank == 3 || rank == 0){
    std::cout << "Shouldn't happen! rank == 3 or rank ==0 " << type_by_number(type_) << std::endl;
    return ret;
  }
  // fill in the diagonalized upper 3x3
  for(size_t i = 0; i<3; ++i)
    ret[i][i] = lambda[i];
  //compute transformed quadric matrix g, h, i coefficients gp, hp, ip
  // where,
  //    gp         g
  //    hp = 1/2 E h
  //    ip         i

 T gp = (E[0][0]*g_ + E[0][1]*h_ + E[0][2]*i_)/T(2);
 T hp = (E[1][0]*g_ + E[1][1]*h_ + E[1][2]*i_)/T(2);
 T ip = (E[2][0]*g_ + E[2][1]*h_ + E[2][2]*i_)/T(2);

  std::vector<bool> t_known(3, false);
  T sum = T(0);
  if(lambda[0] != T(0)){
    tr[0] = gp/lambda[0];
    t_known[0]=true;
    sum += gp*tr[0];
  }else{
    ret[0][3] = gp;
    ret[3][0] = gp;
  }
  if(lambda[1] != T(0)){
    tr[1] = hp/lambda[1];
    t_known[1]=true;
    sum += hp*tr[1];
  }else{
    ret[1][3] = hp;
    ret[3][1] = hp;
  }
  if(lambda[2] != T(0)){
    tr[2] = ip/lambda[2];
    t_known[2]=true;
    sum += ip*tr[2];
  }else{
    ret[2][3] = ip;
    ret[3][2] = ip;
  }
  // for rank == 2, determine the one remaining
  // unknown translation component to set j' = 0
  // consider the possibility that g,h,i, coefficients
  // are too small to be used (i.e. < rtol)
  if(rank == 2){
    if(!t_known[0]){
      if(fabs(gp) < rtol){
        ret[3][3] = j_;
        ret[0][3] = T(0);
        ret[3][0] = T(0);
      }else{
        tr[0] = (j_ - sum)/(T(2)*gp);
      }
    }else if(!t_known[1]){
      if(fabs(hp) < rtol){
        ret[3][3] = j_;
        ret[1][3] = T(0);
        ret[3][1] = T(0);
      }else{
        tr[1] = (j_ - sum)/(T(2)*hp);
      }
    }else if(!t_known[2]){
      if(fabs(ip) < rtol){
        ret[3][3] = j_;
        ret[2][3] = T(0);
        ret[3][2] = T(0);
      }else{
        tr[2] = (j_ - sum)/(T(2)*ip);
      }
    }
  }
  // for rank == 1, determine the two translational
  // degrees of freedom to reduce j to zero
  // consider the possibility that g,h,i, coefficients
  // are too small to be used (i.e. < rtol)
  if(rank == 1){
    // handle all cases of small coefficients, tedious but necessary
    if(!t_known[0]&&!t_known[1]){
      if((fabs(gp) < rtol)&& (fabs(hp) < rtol)){
        ret[3][3] = j_;
      }else if((fabs(gp) >=  rtol) && (fabs(hp) >= rtol)){
        T temp = (j_ - sum)/(T(2)*(gp+hp));
        tr[0] = temp; tr[1] = temp;
      }else if((fabs(gp) >= rtol)&&(fabs(hp) < rtol)){
        tr[0] = (j_ - sum)/(T(2)*gp);
        ret[1][3] = T(0);
        ret[3][1] = T(0);
      }else if((fabs(gp) < rtol)&&(fabs(hp) >= rtol)){
        tr[1] = (j_ - sum)/(T(2)*hp);
        ret[0][3] = T(0);
        ret[3][0] = T(0);
      }
    }else if(!t_known[0]&&!t_known[2]){
      if((fabs(gp) < rtol)&& (fabs(ip) < rtol)){
        ret[3][3] = j_;
      }else if((fabs(gp) >=  rtol) && (fabs(ip) >= rtol)){
        T temp = (j_ - sum)/(T(2)*(gp+ip));
        tr[0] = temp; tr[2] = temp;
      }else if((fabs(gp) >= rtol)&&(fabs(ip) < rtol)){
        tr[0] = (j_ - sum)/(T(2)*gp);
        ret[2][3] = T(0);
        ret[3][2] = T(0);
      }else if((fabs(gp) < rtol)&&(fabs(ip) >= rtol)){
        tr[2] = (j_ - sum)/(T(2)*ip);
        ret[0][3] = T(0);
        ret[3][0] = T(0);
      }
    }else if(!t_known[1]&&!t_known[2]){
      if((fabs(hp) < rtol) && (fabs(ip) < rtol)){
        ret[3][3] = j_;
      }else if((fabs(hp) >=  rtol) && (fabs(ip) >=  rtol)){
        T temp = (j_ - sum)/(T(2)*(hp+ip));
        tr[1] = temp; tr[2] = temp;
      }else if((fabs(hp) >= rtol) && (fabs(ip) < rtol)){
        tr[1] = (j_ - sum)/(T(2)*hp);
        ret[2][3] = T(0);
        ret[3][2] = T(0);
      }else if((fabs(hp) < rtol) && (fabs(ip) >= rtol)){
        tr[2] = (j_ - sum)/(T(2)*ip);
        ret[1][3] = T(0);
        ret[3][1] = T(0);
      }
    }
  }
  H[0][3] = -(E[0][0]*tr[0] + E[1][0]*tr[1] + E[2][0]*tr[2]);
  H[1][3] = -(E[0][1]*tr[0] + E[1][1]*tr[1] + E[2][1]*tr[2]);
  H[2][3] = -(E[0][2]*tr[0] + E[1][2]*tr[1] + E[2][2]*tr[2]);
  return ret;
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
  while(true){
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
