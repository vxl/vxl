// This is core/vgl/algo/vgl_fit_quadric_3d.hxx
#ifndef vgl_fit_quadric_3d_hxx_
#define vgl_fit_quadric_3d_hxx_
//:
// \file
#include <limits>
#include <iostream>
#include <vnl/vnl_math.h>
#include "vgl_fit_quadric_3d.h"
#include <vgl/algo/vgl_norm_trans_3d.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_matrix.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_distance.h>
#include <vgl/vgl_tolerance.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/algo/vnl_generalized_eigensystem.h>
#include <vnl/algo/vnl_real_eigensystem.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vnl/vnl_least_squares_function.h>
#define debug 0

template <class T>
vgl_fit_quadric_3d<T>::vgl_fit_quadric_3d(std::vector<vgl_point_3d<T> > points)

{
  for(typename std::vector<vgl_point_3d<T> >::iterator pit = points.begin();
      pit != points.end(); ++pit)
    points_.push_back(vgl_homg_point_3d<T>(*pit));
}

template <class T>
void vgl_fit_quadric_3d<T>::add_point(vgl_point_3d<T> const &p)
{
  points_.push_back(vgl_homg_point_3d<T>(p));
}

template <class T>
void vgl_fit_quadric_3d<T>::add_point(const T x, const T y, const T z)
{
  points_.push_back(vgl_homg_point_3d<T>(x, y, z));
}

 template <class T>
 void vgl_fit_quadric_3d<T>::clear()
{
  points_.clear();
}

template <class T>
std::vector<vgl_point_3d<T> > vgl_fit_quadric_3d<T>::get_points() const{
  std::vector<vgl_point_3d<T> > ret;
  const unsigned n = static_cast<unsigned>(points_.size());
  for (unsigned i=0; i<n; i++){
    vgl_point_3d<T> p(points_[i]);
    ret.push_back(p);
  }
  return ret;
}

template <class T>
T vgl_fit_quadric_3d<T>::fit_linear_Taubin(std::ostream* errstream)
{
  size_t n = points_.size();
  // actually the minimum number is 9 but require at least 10 to get some amount of regularization
  if(n<10){
    if (errstream)
      *errstream << "Insufficient number of points to fit  quadric " << n << std::endl;
    return T(-1);
  }
    // normalize the points
  vgl_norm_trans_3d<T> norm;
  if (!norm.compute_from_points(points_) && errstream) {
    *errstream << "there is a problem with norm transform\n";
    return T(-1);
  }

  vnl_matrix<double> M(10,10, 0.0), N(10,10, 0.0);
  for (size_t i=0; i<n; i++) {
    vgl_homg_point_3d<T> hp = norm(points_[i]);//normalize
    // ax^2 + by^2 + cz^2 + dxy + exz + fyz + gxw +hyw +izw +jw^2 = 0
    // let q = {a , b , c , d , e , f , g ,h ,i ,j}
    // let p = {x^2 , y^2 , z^2 , xy , xz , yz , xw , yw , zw, w^2}
    // then q^t p = algebraic distance
    vnl_matrix<double> p(10, 1), dx(10,1,0.0), dy(10,1,0.0), dz(10,1,0.0);
    double x = static_cast<double>(hp.x()), y = static_cast<double>(hp.y()),
      z = static_cast<double>(hp.z()), w = static_cast<double>(hp.w());
    p[0][0]= x*x; p[1][0]=y*y; p[2][0]=z*z; p[3][0]=x*y; p[4][0]=x*z;
    p[5][0]= y*z; p[6][0]=x*w; p[7][0]=y*w; p[8][0]=z*w; p[9][0]=w*w;
    dx[0][0] = 2.0*x; dx[3][0] = y; dx[4][0] = z; dx[6][0]=w;
    dy[1][0] = 2.0*y; dy[3][0] = x; dy[5][0] = z; dy[7][0]=w;
    dz[2][0] = 2.0*z; dz[4][0] = x; dz[5][0] = y; dz[8][0]=w;
    M += p*(p.transpose());
    N += (dx*(dx.transpose()) + dy*(dy.transpose()) + dz*(dz.transpose()));
  }
  M/=n; N/=n;

  // add a small offset to make N positive definite
  double tol =  100.0*vgl_tolerance<double>::position;
  N[9][9] = tol;

  // solve the generalized eigenvalue problem (M-lambda*N)q=0
  vnl_generalized_eigensystem gev(M, N);

  // extract the quadric coefficients
  vnl_vector<T> q(10);
  // The solution, q,  is the eigenvector corresponding to the
  // minimum eigenvalue (index = 0);
  for(size_t r = 0; r<10; ++r)
    q[r] = static_cast<T>((gev.V)[r][0]);


  if(debug) std::cout << "q\n" << q << std::endl;

  // Transform the quadric back to the original coordinate frame
  quadric_Taubin_.set(q[0],q[1],q[2],q[3],q[4],q[5],q[6],q[7],q[8],q[9]);
  std::vector<std::vector<T> > qq = quadric_Taubin_.coef_matrix();
  vnl_matrix<T> Q(4,4);
  for(size_t r = 0; r<4; ++r)
    for(size_t c = 0; c<4; ++c)
      Q[r][c] = qq[r][c];
  vnl_matrix<T> Tr = norm.get_matrix();
  vnl_matrix<T> Qorig(4, 4);
  Qorig = Tr.transpose() * Q * Tr;
  for(size_t r = 0; r<4; ++r)
    for(size_t c = 0; c<4; ++c)
      qq[r][c] = Qorig[r][c];

  quadric_Taubin_.set(qq);
  if(debug) std::cout << quadric_Taubin_.type_by_number(quadric_Taubin_.type()) <<std::endl;

  // compute average sampson distance error for the original point set
  T dsum = 0.0;
  for (unsigned i=0; i<n; i++) {
    T d = quadric_Taubin_.sampson_dist(points_[i]);
    dsum += d;
  }
  return static_cast<T>(dsum/n);
}

template <class T>
T vgl_fit_quadric_3d<T>::fit_ellipsoid_linear_Allaire(std::ostream* errstream){
  size_t n = points_.size();
  // actually the minimum number is 9 but require at least 10 to get some amount of regularization
  if(n<10){
    if (errstream)
      *errstream << "Insufficient number of points to fit  quadric " << n << std::endl;
    return T(-1);
  }
    // normalize the points
  vgl_norm_trans_3d<T> norm;
  if (!norm.compute_from_points(points_) && errstream) {
    *errstream << "there is a problem with norm transform\n";
    return T(-1);
  }

  vnl_matrix<double> M(10,10, 0.0), N(10,10, 0.0);
  for (size_t i=0; i<n; i++) {
    vgl_homg_point_3d<T> hp = norm(points_[i]);//normalize
    // ax^2 + by^2 + cz^2 + dxy + exz + fyz + gxw +hyw +izw +jw^2 = 0
    // let q = {a , b , c , d , e , f , g ,h ,i ,j}
    // let p = {x^2 , y^2 , z^2 , xy , xz , yz , xw , yw , zw, w^2}
    // then q^t p = algebraic distance
    vnl_matrix<double> p(10, 1);
    double x = static_cast<double>(hp.x()), y = static_cast<double>(hp.y()),
      z = static_cast<double>(hp.z()), w = static_cast<double>(hp.w());
    p[0][0]= x*x; p[1][0]=y*y; p[2][0]=z*z; p[3][0]=x*y; p[4][0]=x*z;
    p[5][0]= y*z; p[6][0]=x*w; p[7][0]=y*w; p[8][0]=z*w; p[9][0]=w*w;
    M += p*(p.transpose());
  }
  M/=n;
  // condition the M matrix to be positive definite
  double tol = 100.0*vgl_tolerance<double>::position;
  for(size_t i = 0; i<10; ++i)
    M[i][i]+=tol;
  // for ellipsoid the constraint is as follows:
  // let the coeficient matrix
  //      _     _
  //  Q = |U   b|
  //      |b^t j|
  //      -    -
  // so that x^t Q x + bx + j = 0 is the general equation of a quadric
  // The sum of eigenvalues of U is given by Tr(U)
  // the product of eigenvalues by |U| (determinant)
  // and the sum of pairwise eigenvalue products by  SU2x2 = U2x2_0 + U2x2_1 + U2x2_2
  // where U2x2_i are the principal minors of U.
  //
  // Allaire et al show that the constraint 4*SU2x2 - Tr(U) = 1
  // guarantees that one of the solutions below corresponds to an ellipsoid.

  // this constraint is equivalent to q^t N q = 1 where,
    N[0][0]= -1.0; N[0][1]=  1.0; N[0][2]=  1.0;
    N[1][0]= 1.0; N[1][1]= -1.0; N[1][2]=  1.0;
    N[2][0]= 1.0; N[2][1]=  1.0; N[2][2]= -1.0;
    N[3][3]= -1.0; N[4][4]=  -1.0; N[5][5]= -1.0;

  // N has eigenvalues [-1 -1 -1 -1 -1 0 0 0 0 0 1], i.e singular
  // The eigenvalues of the solution eigenvectors will have the same signs

  // since N is singular it is necessary to solve the
  // generalized eigensystem (N - lambda M)q = 0
  // in this case the solution is the eigenvector which corresponds to an ellipsoid
  // and has the least normalized error, q^t M q / q^t N q .
  vnl_generalized_eigensystem geign(N, M);

  // extract the quadric coefficients
  size_t min_index = 10;
  vnl_matrix<double> q(10,1);
  vnl_diag_matrix<double> D = geign.D;
  double min_error = std::numeric_limits<double>::max();
  for(size_t i = 0; i<5; ++i){
    for(size_t r = 0; r<10; ++r)
      q[r][0] = static_cast<T>((geign.V)[r][i])/sqrt(fabs(D[i]));

    quadric_Allaire_.set(q[0][0],q[1][0],q[2][0],q[3][0],q[4][0],q[5][0],q[6][0],q[7][0],q[8][0],q[9][0]);
    if(quadric_Allaire_.type() != vgl_quadric_3d<T>::real_ellipsoid)
      continue;
    vnl_matrix<double> neu =  q.transpose() * M * q;
        vnl_matrix<double> den  = q.transpose() * N * q;
    double e = fabs(neu[0][0])/fabs(den[0][0]);
   if(debug)  std::cout << "Error =" << e << "for q[" << i << "]\n" << q << std::endl;
    if(e<min_error){
      min_index = i;
      min_error = e;
    }
  }
  for(size_t r = 0; r<10; ++r)
    q[r][0] = static_cast<T>((geign.V)[r][9])/sqrt(fabs(D[9]));

  quadric_Allaire_.set(q[0][0],q[1][0],q[2][0],q[3][0],q[4][0],q[5][0],q[6][0],q[7][0],q[8][0],q[9][0]);
  if(quadric_Allaire_.type() == vgl_quadric_3d<T>::real_ellipsoid){
    vnl_matrix<double> neu =  q.transpose() * M * q;
    vnl_matrix<double> den  = q.transpose() * N * q;
    double e = fabs(neu[0][0])/fabs(den[0][0]);
    if(debug) std::cout << "Error =" << e << " for q[" << 9 << "]\n" << q << std::endl;
    if(e<min_error){
      min_error = e;
      min_index = 9;
    }
  }
  if(min_index == 10){
    std::cout << "failed to find ellipsoid" << std::endl;
    return T(-1);
  }
  if(debug) std::cout << "Best ellipsoid is " << min_index << std::endl;
  for(size_t r = 0; r<10; ++r)
    q[r][0] = static_cast<T>((geign.V)[r][min_index])/sqrt(fabs(D[min_index]));

  // Transform the quadric back to the original coordinate frame
  quadric_Allaire_.set(q[0][0],q[1][0],q[2][0],q[3][0],q[4][0],q[5][0],q[6][0],q[7][0],q[8][0],q[9][0]);
  std::vector<std::vector<T> > qq = quadric_Allaire_.coef_matrix();
  vnl_matrix<T> Q(4,4);
  for(size_t r = 0; r<4; ++r)
    for(size_t c = 0; c<4; ++c)
      Q[r][c] = qq[r][c];
  vnl_matrix<T> Tr = norm.get_matrix();
  vnl_matrix<T> Qorig(4, 4);
  Qorig = Tr.transpose() * Q * Tr;
  for(size_t r = 0; r<4; ++r)
    for(size_t c = 0; c<4; ++c)
      qq[r][c] = Qorig[r][c];

  quadric_Allaire_.set(qq);
  std::cout << quadric_Allaire_.type_by_number(quadric_Allaire_.type()) <<std::endl;

  // compute average sampson distance error for the original point set
  T dsum = 0.0;
  for (unsigned i=0; i<n; i++) {
    T d = quadric_Allaire_.sampson_dist(points_[i]);
    dsum += d;
  }
  return static_cast<T>(dsum/n);
}
template <class T>
T vgl_fit_quadric_3d<T>::fit_saddle_shaped_quadric_linear_Allaire(std::ostream* errstream){
  size_t n = points_.size();
  // actually the minimum number is 9 but require at least 10 to get some amount of regularization
  if(n<10){
    if (errstream)
      *errstream << "Insufficient number of points to fit  quadric " << n << std::endl;
    return T(-1);
  }
    // normalize the points
  vgl_norm_trans_3d<T> norm;
  if (!norm.compute_from_points(points_) && errstream) {
    *errstream << "there is a problem with norm transform\n";
    return T(-1);
  }

  vnl_matrix<double> M(10,10, 0.0), N(10,10, 0.0);
  for (size_t i=0; i<n; i++) {
    vgl_homg_point_3d<T> hp = norm(points_[i]);//normalize
    // ax^2 + by^2 + cz^2 + dxy + exz + fyz + gxw +hyw +izw +jw^2 = 0
    // let q = {a , b , c , d , e , f , g ,h ,i ,j}
    // let p = {x^2 , y^2 , z^2 , xy , xz , yz , xw , yw , zw, w^2}
    // then q^t p = algebraic distance
    vnl_matrix<double> p(10, 1);
    double x = static_cast<double>(hp.x()), y = static_cast<double>(hp.y()),
      z = static_cast<double>(hp.z()), w = static_cast<double>(hp.w());
    p[0][0]= x*x; p[1][0]=y*y; p[2][0]=z*z; p[3][0]=x*y; p[4][0]=x*z;
    p[5][0]= y*z; p[6][0]=x*w; p[7][0]=y*w; p[8][0]=z*w; p[9][0]=w*w;
    M += p*(p.transpose());
  }
  M/=n;
  // condition the M matrix to be positive definite
  double tol = 100.0*vgl_tolerance<double>::position;
  for(size_t i = 0; i<10; ++i)
    M[i][i]+=tol;

  // for saddle shaped quadrics the constraint is as follows:
  // let the coeficient matrix
  //      _     _
  //  Q = |U   b|
  //      |b^t j|
  //      -    -
  // so that x^t Q x + bx + j = 0 is the general equation of a quadric
  // The sum of eigenvalues of U is given by Tr(U)
  // the product of eigenvalues by |U| (determinant)
  // and the sum of pairwise eigenvalue products by  SU2x2 = U2x2_0 + U2x2_1 + U2x2_2
  // where U2x2_i are the principal minors of U.
  //
  // Allaire et al show that the constraint SU2x2 = -1 and q^t N q = -1
  // guarantees that the solution with a negative eigenvalue will fall in the
  // class of saddle shaped surfaces

  // the quadradic normalizing constraint is equivalent to q^t N q = -1 where,
    N[0][0]= 0.0; N[0][1]=  0.5; N[0][2]=  0.5;
    N[1][0]= 0.5; N[1][1]=  0.0; N[1][2]=  0.5;
    N[2][0]= 0.5; N[2][1]=  0.5; N[2][2]= 0.0;
    N[3][3]= -0.25; N[4][4]=  -0.25; N[5][5]= -0.25;

    // N has eigenvalues [-0.5 -0.5 -0.25 -0.25 -0.25 0 0 0 0 1], i.e singular
  // The eigenvalues of the generalized eigensystem will have the same signs

  // since N is singular it is necessary to solve the
  // generalized eigensystem (N - lambda M)q = 0
  // in this case the solution is the eigenvector with a
  // negative eigenvalue and with the least error q^t M q / q^t N q .
  vnl_generalized_eigensystem geign(N, M);
  if(debug) std::cout << geign.D << std::endl;
  if(debug) std::cout << geign.V << std::endl;

  // extract the quadric coefficients
  size_t min_index = 10;
  vnl_matrix<double> q(10,1);
  vnl_diag_matrix<double> D = geign.D;
  double min_error = std::numeric_limits<double>::max();
  for(size_t i = 0; i<5; ++i){
    for(size_t r = 0; r<10; ++r)
      q[r][0] = (geign.V)[r][i]/sqrt(fabs(D[i]));

    quadric_Allaire_.set(q[0][0],q[1][0],q[2][0],q[3][0],q[4][0],q[5][0],q[6][0],q[7][0],q[8][0],q[9][0]);
    vnl_matrix<double> neu =  q.transpose() * M * q;
    vnl_matrix<double> den  = q.transpose() * N * q;
    double e = fabs(neu[0][0])/fabs(den[0][0]);
   if(debug)  std::cout << "Error =" << e << "for q[" << i << "]\n" << q << std::endl;
   if(debug)  std::cout << "Type "<< quadric_Allaire_.type_by_number(quadric_Allaire_.type()) <<std::endl;
    if(e<min_error){
      min_index = i;
      min_error = e;
    }
  }
  if(min_index == 10){
    std::cout << "failed to find saddle shape" << std::endl;
    return T(-1);
  }
  if(debug) std::cout << "Best saddle shape is " << min_index << std::endl;
  for(size_t r = 0; r<10; ++r)
    q[r][0] =(geign.V)[r][min_index]/sqrt(fabs(D[min_index]));

  // Transform the quadric back to the original coordinate frame
  quadric_Allaire_.set(q[0][0],q[1][0],q[2][0],q[3][0],q[4][0],q[5][0],q[6][0],q[7][0],q[8][0],q[9][0]);
  std::vector<std::vector<T> > qq = quadric_Allaire_.coef_matrix();
  vnl_matrix<T> Q(4,4);
  for(size_t r = 0; r<4; ++r)
    for(size_t c = 0; c<4; ++c)
      Q[r][c] = qq[r][c];
  vnl_matrix<T> Tr = norm.get_matrix();
  vnl_matrix<T> Qorig(4, 4);
  Qorig = Tr.transpose() * Q * Tr;
  for(size_t r = 0; r<4; ++r)
    for(size_t c = 0; c<4; ++c)
      qq[r][c] = Qorig[r][c];

  quadric_Allaire_.set(qq);
  std::cout << quadric_Allaire_.type_by_number(quadric_Allaire_.type()) <<std::endl;

  // compute average sampson distance error for the original point set
  T dsum = 0.0;
  for (unsigned i=0; i<n; i++) {
    T d = quadric_Allaire_.sampson_dist(points_[i]);
    dsum += d;
  }
  return static_cast<T>(dsum/n);
}

//--------------------------------------------------------------------------
#undef VGL_FIT_QUADRIC_3D_INSTANTIATE
#define VGL_FIT_QUADRIC_3D_INSTANTIATE(T) \
template class vgl_fit_quadric_3d<T >

#endif // vgl_fit_quadric_3d_hxx_
