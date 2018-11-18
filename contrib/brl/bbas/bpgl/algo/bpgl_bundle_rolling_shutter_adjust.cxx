// This is bbas/bpgl/algo/bpgl_bundle_rolling_shutter_adjust.cxx
#include <iostream>
#include <fstream>
#include <algorithm>
#include "bpgl_bundle_rolling_shutter_adjust.h"
//:
// \file

#include <vnl/vnl_vector_ref.h>
#include <vnl/vnl_double_3.h>
#include <vnl/algo/vnl_sparse_lm.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vnl/vnl_double_4.h>

vnl_double_4x4
bpgl_bundle_rolling_shutter_adj_lsqr::param_to_motion_matrix(int /*i*/, const double* data, double r, double v) const
{
  double t=v/r;

  double omegas[6]={0,0,0,0,0,0};
  omegas[0]=data[6];omegas[1]=data[7];omegas[2]=data[8];
  double omegasnorm=std::sqrt(omegas[0]*omegas[0]+omegas[1]*omegas[1]+omegas[2]*omegas[2]);

  vnl_double_3x3 I; I.set_identity();

  vnl_double_3x3 wx=vector_to_skewmatrix(omegas);
  vnl_double_3 d;
  d[0]=data[9];
  d[1]=data[10];
  d[2]=data[11];

  vnl_double_3x3 Mtopleft;
  vnl_double_3   Mtopright;

  Mtopleft =I;
  Mtopright=d*t;
  if (omegasnorm!=0.0)
  {
    double s = std::sin(omegasnorm*t)/omegasnorm;
    double osqrinv=1.0/(omegasnorm*omegasnorm);
    double c = (1.0-std::cos(omegasnorm*t))*osqrinv;
    Mtopleft += wx*(I*s+wx*c);
    Mtopright+= wx*(wx*d*(s+t)*osqrinv -d*c);
  }

  return vnl_double_4x4().set_identity().set_columns(0,Mtopleft).set_column(3,Mtopright);
#if 0 // was:
  M(0,0)=Mtopleft(0,0);M(0,1)=Mtopleft(0,1);M(0,2)=Mtopleft(0,2);M(0,3)=Mtopright(0);
  M(1,0)=Mtopleft(1,0);M(1,1)=Mtopleft(1,1);M(1,2)=Mtopleft(1,2);M(1,3)=Mtopright(1);
  M(2,0)=Mtopleft(2,0);M(2,1)=Mtopleft(2,1);M(2,2)=Mtopleft(2,2);M(2,3)=Mtopright(2);
  M(3,0)=0;            M(3,1)=0;            M(3,2)=0;            M(3,3)=1;
  return M;
#endif
}

//: Constructor
bpgl_bundle_rolling_shutter_adj_lsqr::
  bpgl_bundle_rolling_shutter_adj_lsqr(const std::vector<vpgl_calibration_matrix<double> >& K,
                                       const std::vector<vgl_point_2d<double> >& image_points,
                                       const std::vector<std::vector<bool> >& mask,
                                       double rolling_rate,
                                       bool use_confidence_weights)
 : vnl_sparse_lst_sqr_function(K.size(),12,mask[0].size(),3,0,mask,2,use_gradient),
   K_(K),
   image_points_(image_points),
   use_covars_(false),
   use_weights_(use_confidence_weights),
   weights_(image_points.size(),1.0),
   iteration_count_(0),
   rolling_rate_(rolling_rate)
{
  for (auto & i : K_)
    Km_.push_back(i.get_matrix());
}


//: Constructor
//  Each image point is assigned an inverse covariance (error projector) matrix
// \note image points are not homogeneous because they require finite points to measure projection error
bpgl_bundle_rolling_shutter_adj_lsqr::
bpgl_bundle_rolling_shutter_adj_lsqr(const std::vector<vpgl_calibration_matrix<double> >& K,
                                     const std::vector<vgl_point_2d<double> >& image_points,
                                     const std::vector<vnl_matrix<double> >& inv_covars,
                                     const std::vector<std::vector<bool> >& mask,
                                     bool use_confidence_weights)
 : vnl_sparse_lst_sqr_function(K.size(),6,mask[0].size(),3,0,mask,2,use_gradient),
   K_(K),
   image_points_(image_points),
   use_covars_(true),
   use_weights_(use_confidence_weights),
   weights_(image_points.size(),1.0),
   iteration_count_(0)
{
  for (auto & i : K_)
    Km_.push_back(i.get_matrix());

  assert(image_points.size() == inv_covars.size());
  vnl_matrix<double> U(2,2,0.0);
  for (const auto & S : inv_covars)
  {
    if (S(0,0) > 0.0) {
      U(0,0) = std::sqrt(S(0,0));
      U(0,1) = S(0,1)/U(0,0);
      double U11 = S(1,1)-S(0,1)*S(0,1)/S(0,0);
      U(1,1) = (U11>0.0)?std::sqrt(U11):0.0;
    }
    else if (S(1,1) > 0.0) {
      assert(S(0,1) == 0.0);
      U(0,0) = 0.0;
      U(0,1) = 0.0;
      U(1,1) = std::sqrt(S(1,1));
    }
    else {
      std::cout << "warning: not positive definite"<<std::endl;
      U.fill(0.0);
    }
    factored_inv_covars_.push_back(U);
  }
}


//: Compute all the reprojection errors
//  Given the parameter vectors a and b, compute the vector of residuals e.
//  e has been sized appropriately before the call.
//  The parameters in a for each camera are {wx, wy, wz, tx, ty, tz}
//  where w is the Rodrigues vector of the rotation and t is the translation.
//  The parameters in b for each 3D point are {px, py, pz}
//  the non-homogeneous position.
void
bpgl_bundle_rolling_shutter_adj_lsqr::f(vnl_vector<double> const& a,
                                        vnl_vector<double> const& b,
                                        vnl_vector<double>& e)
{
  for (unsigned int i=0; i<number_of_a(); ++i)
  {
    // Construct the ith camera
    vnl_double_3x4 Pi = param_to_cam_matrix(i,a);

    vnl_crs_index::sparse_vector row = residual_indices_.sparse_row(i);
    for (auto & r_itr : row)
    {
      unsigned int j = r_itr.second;
      unsigned int k = r_itr.first;

      // Construct the jth point
      const double * bj = b.data_block()+index_b(j);
      vnl_vector_fixed<double,4> Xj(bj[0], bj[1], bj[2], 1.0);

      vnl_double_4x4 Mi = param_to_motion_matrix(i,a,rolling_rate_,image_points_[k].y());
      vnl_double_3 P3=Xj.extract(3);
      vnl_double_3 d;
      d[0]=a[i*12+9];
      d[1]=a[i*12+10];
      d[2]=a[i*12+11];
      vnl_double_3x3 KR=Pi.extract(3,3);
      vnl_double_3x1 KT=Pi.extract(3,1,0,3);
      vnl_double_3 KRd=KR*d;
      vnl_double_3 KRP=KR*P3;

      double b=(KRP[2]-KRd[1]/rolling_rate_+KT(2,0));
      double a1=KRd[2]/rolling_rate_;
      double c=-(KRP[1]+KT(1,0));
      double v_s1=(-b+std::sqrt(b*b-4*a1*c))/(2*a1);
      double v_s2=(-b-std::sqrt(b*b-4*a1*c))/(2*a1);

      double v_k=image_points_[k].y();
      if (v_s1>0 && v_s1<1400)
        v_k=v_s1;
      if (v_s2>0 && v_s2<1400)
        v_k=v_s2;
      Mi=param_to_motion_matrix(i,a,rolling_rate_,v_k);

      // Project jth point with the ith camera
      vnl_vector_fixed<double,3> xij = Pi*Mi*Xj;

      double* eij = e.data_block()+index_e(k);
      eij[0] = xij[0]/xij[2] - image_points_[k].x();
      eij[1] = xij[1]/xij[2] - image_points_[k].y();
      if (use_covars_) {
        // multiple this error by upper triangular Sij
        vnl_matrix<double>& Sij = factored_inv_covars_[k];
        eij[0] *= Sij(0,0);
        eij[0] += eij[1]*Sij(0,1);
        eij[1] *= Sij(1,1);
      }
    }
  }

  if (use_weights_) {
    vnl_vector<double> unweighted(e);
    for (unsigned int k=0; k<weights_.size(); ++k) {
      e[2*k]   *= weights_[k];
      e[2*k+1] *= weights_[k];
    }
    // weighted average error
    double avg_error = e.rms();
    for (unsigned int k=0; k<weights_.size(); ++k) {
      vnl_vector_ref<double> uw(2,unweighted.data_block()+2*k);
      double update = 2.0*avg_error/uw.rms();
      if (update < 1.0)
        weights_[k] = std::min(weights_[k], update);
      else
        weights_[k] = 1.0;
      //std::cout << weights_[k] << ' ';
      e[2*k]   = unweighted[2*k]   * weights_[k];
      e[2*k+1] = unweighted[2*k+1] * weights_[k];
    }
    std::cout << std::endl;
  }
}


//: Compute the residuals from the ith component of a and the jth component of b.
//  This is not normally used because f() has a self-contained efficient implementation
//  It is used for finite-differencing if the gradient is marked as unavailable
void
bpgl_bundle_rolling_shutter_adj_lsqr::fij(int i, int j, vnl_vector<double> const& ai,
                                          vnl_vector<double> const& bj, vnl_vector<double>& fij)
{
  // Construct the ith camera
  vnl_double_3x4 Pi = param_to_cam_matrix(i,ai.data_block());

  // Construct the jth point
  vnl_vector_fixed<double,4> Xj(bj[0], bj[1], bj[2], 1.0);

  // Project jth point with the ith camera
  vnl_vector_fixed<double,3> xij = Pi*Xj;

  int k = residual_indices_(i,j);
  fij[0] = xij[0]/xij[2] - image_points_[k].x();
  fij[1] = xij[1]/xij[2] - image_points_[k].y();
  if (use_covars_) {
    // multiple this error by upper triangular Sij
    vnl_matrix<double>& Sij = factored_inv_covars_[k];
    fij[0] *= Sij(0,0);
    fij[0] += fij[1]*Sij(0,1);
    fij[1] *= Sij(1,1);
  }
  if (use_weights_) {
    fij[0] *= weights_[k];
    fij[1] *= weights_[k];
  }
}


//: Compute the sparse Jacobian in block form.
void
bpgl_bundle_rolling_shutter_adj_lsqr::jac_blocks(vnl_vector<double> const& a, vnl_vector<double> const& b,
                                                 std::vector<vnl_matrix<double> >& A,
                                                 std::vector<vnl_matrix<double> >& B)
{
  for (unsigned int i=0; i<number_of_a(); ++i)
  {
    // Construct the ith camera
    vnl_double_3x4 Pi = param_to_cam_matrix(i,a);

    // This is semi const incorrect - there is no vnl_vector_ref_const
    const vnl_vector_ref<double> ai(number_of_params_a(i),
                                    const_cast<double*>(a.data_block())+index_a(i));

    vnl_crs_index::sparse_vector row = residual_indices_.sparse_row(i);
    for (auto & r_itr : row)
    {
      unsigned int j = r_itr.second;
      unsigned int k = r_itr.first;
      // This is semi const incorrect - there is no vnl_vector_ref_const
      const vnl_vector_ref<double> bj(number_of_params_b(j),
                                      const_cast<double*>(b.data_block())+index_b(j));
      double v_k=image_points_[k].y();
      jac_Bij(Pi,ai,bj,v_k,B[k]);   // compute Jacobian B_ij
      jac_Aij(Pi,Km_[i],ai,bj,v_k,A[k]); // compute Jacobian A_ij

      if (use_covars_) {
        const vnl_matrix<double>& Sij = factored_inv_covars_[k];
        A[k] = Sij*A[k];
        B[k] = Sij*B[k];
      }
      if (use_weights_) {
        A[k] *= weights_[k];
        B[k] *= weights_[k];
      }
    }
  }
}


//: compute the Jacobian Aij
void
bpgl_bundle_rolling_shutter_adj_lsqr::jac_Aij(vnl_double_3x4 const& Pi,
                                              vnl_double_3x3 const& K,
                                              vnl_vector<double> const& ai,
                                              vnl_vector<double> const& bj,
                                              double v_k,
                                              vnl_matrix<double>& Aij)
{
  // The translation part.
  //=====================
  // compute by swapping the role of the translation and point position
  // then reused the jac_Bij code
  vnl_double_4x4 M=param_to_motion_matrix(0,ai,rolling_rate_,v_k);
  vnl_double_4 P;
  P[0]=bj[0];P[1]=bj[1];P[2]=bj[2];P[3]=1.0;
  vnl_double_3 p=Pi*M*P;

  {
    vnl_double_3x4 sPi(Pi);
    vnl_double_3x3 Mi = -sPi.extract(3,3);

    Aij(0,3)=(p[2]*Mi(0,0)-p[0]*Mi(2,0))/(p[2]*p[2]);
    Aij(1,3)=(p[2]*Mi(1,0)-p[1]*Mi(2,0))/(p[2]*p[2]);

    Aij(0,4)=(p[2]*Mi(0,1)-p[0]*Mi(2,1))/(p[2]*p[2]);
    Aij(1,4)=(p[2]*Mi(1,1)-p[1]*Mi(2,1))/(p[2]*p[2]);

    Aij(0,5)=(p[2]*Mi(0,2)-p[0]*Mi(2,2))/(p[2]*p[2]);
    Aij(1,5)=(p[2]*Mi(1,2)-p[1]*Mi(2,2))/(p[2]*p[2]);

    //sPi.set_column(3,-(Mi*bj));
    //// This is semi const incorrect - there is no vnl_vector_ref_const
    //const vnl_vector_ref<double> t(3,const_cast<double*>(ai.data_block())+3);

    //vnl_matrix<double> Aij_sub(2,3);
    //jac_Bij(sPi,ai,t,v_k,Aij_sub);
    //Aij.update(Aij_sub,0,3);
  }

  // The rotation part.
  //==================
  // relative translation vector
  {
    vnl_double_4 temp_mp=M*P;

    vnl_double_3 t(temp_mp[0]-ai[3], temp_mp[1]-ai[4], temp_mp[2]-ai[5]);

    const double& x = ai[0];
    const double& y = ai[1];
    const double& z = ai[2];
    double x2 = x*x, y2 = y*y, z2 = z*z;
    double m2 = x2 + y2 + z2;

    if (m2 == 0.0)
    {
      Aij(0,0) = 0;
      Aij(1,0) = -1;
      Aij(0,1) = 1;
      Aij(1,1) = 0;
      Aij(0,2) = 0;
      Aij(1,2) = 0;
    }
    else
    {
      double m = std::sqrt(m2);  // Rodrigues magnitude = rotation angle
      double c = std::cos(m);
      double s = std::sin(m);

      // common trig terms
      double ct = (1-c)/m2;
      double st = s/m;

      // derivative coefficients for common trig terms
      // ds = d/dx_i{st}/x_i
      // dc = d/dx_i{ct}/x_i
      double dct = s/m/m2;
      double dst = c/m2 - dct;
      dct -=  2*(1-c)/(m2*m2);

      double utc = t[2]*x*z + t[1]*x*y - t[0]*(y2+z2);
      double uts = t[2]*y - t[1]*z;
      double vtc = t[0]*x*y + t[2]*y*z - t[1]*(x2+z2);
      double vts = t[0]*z - t[2]*x;
      double wtc = t[0]*x*z + t[1]*y*z - t[2]*(x2+y2);
      double wts = t[1]*x - t[0]*y;

      // projection of the point into normalized homogeneous coordinates
      // should be equal to inv(K)*Pi*[bj|1]
      double u = ct*utc + st*uts + t[0];
      double v = ct*vtc + st*vts + t[1];
      double w = ct*wtc + st*wts + t[2];

      double w2 = w*w;

      double dw = dct*x*wtc + ct*(t[0]*z - 2*t[2]*x)
                + dst*x*wts + st*t[1];
      Aij(0,0) = (w*(dct*x*utc + ct*(t[2]*z + t[1]*y)
                    + dst*x*uts) - u*dw)/w2;
      Aij(1,0) = (w*(dct*x*vtc + ct*(t[0]*y - 2*t[1]*x)
                    + dst*x*vts - st*t[2]) - v*dw)/w2;

      dw = dct*y*wtc + ct*(t[1]*z - 2*t[2]*y)
          + dst*y*wts - st*t[0];
      Aij(0,1) = (w*(dct*y*utc + ct*(t[1]*x - 2*t[0]*y)
                    + dst*y*uts + st*t[2]) - u*dw)/w2;
      Aij(1,1) = (w*(dct*y*vtc + ct*(t[0]*x + t[2]*z)
                    + dst*y*vts) - v*dw)/w2;

      dw = dct*z*wtc + ct*(t[0]*x + t[1]*y)
          + dst*z*wts;
      Aij(0,2) = (w*(dct*z*utc + ct*(t[2]*x - 2*t[0]*z)
                    + dst*z*uts - st*t[1]) - u*dw)/w2;
      Aij(1,2) = (w*(dct*z*vtc + ct*(t[2]*y - 2*t[1]*z)
                    + dst*z*vts + st*t[0]) - v*dw)/w2;
    }

    Aij(0,0) *= K(0,0);
    Aij(0,0) += Aij(1,0)*K(0,1);
    Aij(1,0) *= K(1,1);
    Aij(0,1) *= K(0,0);
    Aij(0,1) += Aij(1,1)*K(0,1);
    Aij(1,1) *= K(1,1);
    Aij(0,2) *= K(0,0);
    Aij(0,2) += Aij(1,2)*K(0,1);
    Aij(1,2) *= K(1,1);
  }
  //Aij.fill(0.0);
  {
    // The angular velocity part.
    double dx=ai[9],dy=ai[10],dz=ai[11];

    vnl_double_3 d;
    d[0]=dx;
    d[1]=dy;
    d[2]=dz;

    vnl_double_3 P3=P.extract(3);

    vnl_double_3x3 KR=Pi.extract(3,3);
    vnl_double_3x1 KT=Pi.extract(3,1,0,3);
    vnl_double_3 KRd=KR*d;
    vnl_double_3 KRP=KR*P3;

    double b=(KRP[2]-KRd[1]/rolling_rate_+KT(2,0));
    double a=KRd[2]/rolling_rate_;
    double c=-(KRP[1]+KT(1,0));
    double v_s1=(-b+std::sqrt(b*b-4*a*c))/(2*a);
    double v_s2=(-b-std::sqrt(b*b-4*a*c))/(2*a);

    if (v_s1>0 && v_s1<1400)
        v_k=v_s1;
    if (v_s2>0 && v_s2<1400)
        v_k=v_s2;
    M=param_to_motion_matrix(0,ai,rolling_rate_,v_k);
    p=Pi*M*P;

    double dvddx=(p[2]*KR(1,0)-p[1]*KR(2,0))*v_k/rolling_rate_/(p[2]*p[2]-p[2]*KRd[1]/rolling_rate_+p[1]*KRd[2]/rolling_rate_);
    double dupddx=KR(0,0)*v_k/rolling_rate_+KRd[0]/rolling_rate_*dvddx;
    double dwpddx=KR(2,0)*v_k/rolling_rate_+KRd[2]/rolling_rate_*dvddx;
    double duddx=(p[2]*dupddx-p[0]*dwpddx)/(p[2]*p[2]);

    double dvddy=(p[2]*KR(1,1)-p[1]*KR(2,1))*v_k/rolling_rate_/(p[2]*p[2]-p[2]*KRd[1]/rolling_rate_+p[1]*KRd[2]/rolling_rate_);
    double dupddy=KR(0,1)*v_k/rolling_rate_+KRd[0]/rolling_rate_*dvddy;
    double dwpddy=KR(2,1)*v_k/rolling_rate_+KRd[2]/rolling_rate_*dvddy;
    double duddy=(p[2]*dupddy-p[0]*dwpddy)/(p[2]*p[2]);

    double dvddz=(p[2]*KR(1,2)-p[1]*KR(2,2))*v_k/rolling_rate_/(p[2]*p[2]-p[2]*KRd[1]/rolling_rate_+p[1]*KRd[2]/rolling_rate_);
    double dupddz=KR(0,2)*v_k/rolling_rate_+KRd[0]/rolling_rate_*dvddz;
    double dwpddz=KR(2,2)*v_k/rolling_rate_+KRd[2]/rolling_rate_*dvddz;
    double duddz=(p[2]*dupddz-p[0]*dwpddz)/(p[2]*p[2]);

#if 0
    double ox=ai[6],oy=ai[7],oz=ai[8];
    double om=std::sqrt(ox*ox+oy*oy+oz*oz);
    vnl_double_3x3 drdox(0.0),drdoy(0.0),drdoz(0.0);
    vnl_double_3x1 dddox(0.0),dddoy(0.0),dddoz(0.0);

    vnl_double_3x3 oc(0.0);
    oc(0,1)=-oz;oc(0,2)=oy;
    oc(1,0)=oz;oc(1,2)=-ox;
    oc(2,0)=oy;oc(2,1)=ox;

    vnl_double_3x3 oc2=oc*oc;

    vnl_double_3x3 docdox(0.0);
    docdox(1,2)=-1;docdox(2,1)=1;
    vnl_double_3x3 docdoy(0.0);
    docdoy(0,2)=1;docdoy(2,0)=-1;
    vnl_double_3x3 docdoz(0.0);
    docdoz(0,1)=-1;docdoz(1,0)=1;

    vnl_double_3x3 doc2dox(0.0);
    doc2dox(0,1)=oy;doc2dox(0,2)=oz;
    doc2dox(1,0)=oy;doc2dox(1,1)=-2*ox;
    doc2dox(2,0)=oz;doc2dox(2,2)=-2*ox;

    vnl_double_3x3 doc2doy(0.0);
    doc2doy(0,0)=-2*oy;doc2doy(0,1)=ox;
    doc2doy(1,0)=ox;doc2doy(1,2)=oz;
    doc2doy(2,1)=oz;doc2doy(2,2)=-2*oy;

    vnl_double_3x3 doc2doz(0.0);
    doc2doz(0,0)=-2*oz;doc2doz(0,2)=ox;
    doc2doz(1,1)=-2*oz;doc2doz(1,2)=oy;
    doc2doz(2,0)=ox;doc2doz(2,1)=oy;
    if (om!=0.0)
    {
      double domdox=ox/om, domdoy=oy/om, domdoz=oz/om;
      drdox =domdox*t*std::sin(om*t)/(om*om)*oc2 -   domdox*std::sin(om*t)/(om*om)*oc         + std::sin(om*t)/om*docdox
            +domdox*t*std::cos(om*t)/om*oc       - 2*domdox*(1-std::cos(om*t))/(om*om*om)*oc2 +(1-std::cos(om*t))/(om*om)*doc2dox;

      drdoy =domdoy*t*std::sin(om*t)/(om*om)*oc2 -   domdoy*std::sin(om*t)/(om*om)*oc + std::sin(om*t)/om*docdoy
            +domdoy*t*std::cos(om*t)/om*oc       - 2*domdoy*(1-std::cos(om*t))/(om*om*om)*oc2+(1-std::cos(om*t))/(om*om)*doc2doy;

      drdoz =domdoz*t*std::sin(om*t)/(om*om)*oc2 -   domdoz*std::sin(om*t)/(om*om)*oc + std::sin(om*t)/om*docdoz
            +domdoz*t*std::cos(om*t)/om*oc       - 2*domdoz*(1-std::cos(om*t))/(om*om*om)*oc2+(1-std::cos(om*t))/(om*om)*doc2doz;

      dddox =-3*domdox*(std::sin(om*t)+om*t)/(om*om*om*om)*(oc2*d) + (std::sin(om*t)+om*t)/(om*om*om)*(doc2dox*d)
            -domdox*t*(std::sin(om*t))/(om*om)*oc*d + (domdox*t*std::cos(om*t)+domdox*t)*(oc2*d)/(om*om*om)
            +2*domdox*(1-std::cos(om*t))/(om*om*om)*(oc*d)- (1-std::cos(om*t))/(om*om)*docdox*d;

      dddoy =-3*domdoy*(std::sin(om*t)+om*t)/(om*om*om*om)*(oc2*d) + (std::sin(om*t)+om*t)/(om*om*om)*(doc2doy*d)
            -domdoy*t*(std::sin(om*t))/(om*om)*oc*d + (domdoy*t*std::cos(om*t)+domdoy*t)*(oc2*d)/(om*om*om)
            +2*domdoy*(1-std::cos(om*t))/(om*om*om)*(oc*d)- (1-std::cos(om*t))/(om*om)*docdoy*d;

      dddoz =-3*domdoz*(std::sin(om*t)+om*t)/(om*om*om*om)*(oc2*d) + (std::sin(om*t)+om*t)/(om*om*om)*(doc2doz*d)
            -domdoz*t*(std::sin(om*t))/(om*om)*oc*d + (domdoz*t*std::cos(om*t)+domdoz*t)*(oc2*d)/(om*om*om)
            +2*domdoz*(1-std::cos(om*t))/(om*om*om)*(oc*d)- (1-std::cos(om*t))/(om*om)*docdoz*d;
    }

    vnl_double_4x4 dmdox(0.0);
    dmdox.update(drdox,0,0);
    dmdox.update(dddox,0,3);

    vnl_double_4x4 dmdoy(0.0);
    dmdoy.update(drdoy,0,0);
    dmdoy.update(dddoy,0,3);

    vnl_double_4x4 dmdoz(0.0);
    dmdoz.update(drdoz,0,0);
    dmdoz.update(dddoz,0,3);

    vnl_double_3x3 drdd(0.0);

    vnl_double_3x3 KR=Pi.extract(3,3);
    vnl_double_3x1 KRd=KR*d;

    double dvddx=KR(1,0)*v_k/(rolling_rate_-KRd(1,0));
    double duddx=(1/rolling_rate_)*(KR(0,0)*v_k+KRd(0,0)*dvddx);
    double dwddx=(1/rolling_rate_)*(KR(2,0)*v_k+KRd(2,0)*dvddx);

    double dvddy=KR(1,1)*v_k/(rolling_rate_-KRd(1,0));
    double duddy=(1/rolling_rate_)*(KR(0,1)*v_k+KRd(0,0)*dvddy);
    double dwddy=(1/rolling_rate_)*(KR(2,1)*v_k+KRd(2,0)*dvddy);

    double dvddz=KR(1,2)*v_k/(rolling_rate_-KRd(1,0));
    double duddz=(1/rolling_rate_)*(KR(0,2)*v_k+KRd(0,0)*dvddz);
    double dwddz=(1/rolling_rate_)*(KR(2,2)*v_k+KRd(2,0)*dvddz);

    if (om==0.0)
    {
      dtddx= ddddx*t;
      dtddy= ddddy*t;
      dtddz= ddddz*t;
    }
    else
    {
      dtddx= (std::sin(om*t)+om*t)/(om*om*om)*oc2*ddddx- (1-std::cos(om*t))/(om*om)*(oc*ddddx)+ddddx*t;
      dtddy= (std::sin(om*t)+om*t)/(om*om*om)*oc2*ddddy- (1-std::cos(om*t))/(om*om)*(oc*ddddy)+ddddy*t;
      dtddz= (std::sin(om*t)+om*t)/(om*om*om)*oc2*ddddz- (1-std::cos(om*t))/(om*om)*(oc*ddddz)+ddddz*t;
    }
    vnl_double_4x4 dmddx(0.0);dmddx.update(dtddx,0,3);
    vnl_double_4x4 dmddy(0.0);dmddy.update(dtddy,0,3);
    vnl_double_4x4 dmddz(0.0);dmddz.update(dtddz,0,3);

    vnl_double_3 dpdi=Pi*dmdox*P; // , Pi*dmdoy*P, Pi*dmdoz*P, Pi*dmddx*P
#endif // 0

    // 6 motion parameters.
    Aij(0,6)=0.0;
    Aij(1,6)=0;
    Aij(0,7)=0;
    Aij(1,7)=0;
    Aij(0,8)=0;
    Aij(1,8)=0;

    Aij(0,9)= duddx;
    Aij(1,9)= dvddx;
    Aij(0,10)=duddy;
    Aij(1,10)=dvddy;
    Aij(0,11)=duddz;
    Aij(1,11)=dvddz;
  }
}


//: compute the Jacobian Bij
void
bpgl_bundle_rolling_shutter_adj_lsqr::jac_Bij(vnl_double_3x4  Pi,
                                              vnl_vector<double> const& ai,
                                              vnl_vector<double> const& bj,
                                              double v_k,
                                              vnl_matrix<double>& Bij)
{
  vnl_double_4x4 M=param_to_motion_matrix(0,ai,rolling_rate_,v_k);

  Pi=Pi*M;
  double denom = Pi(2,0)*bj[0] + Pi(2,1)*bj[1] + Pi(2,2)*bj[2] + Pi(2,3);
  denom *= denom;

  double txy = Pi(0,0)*Pi(2,1) - Pi(0,1)*Pi(2,0);
  double txz = Pi(0,0)*Pi(2,2) - Pi(0,2)*Pi(2,0);
  double tyz = Pi(0,1)*Pi(2,2) - Pi(0,2)*Pi(2,1);
  double tx  = Pi(0,0)*Pi(2,3) - Pi(0,3)*Pi(2,0);
  double ty  = Pi(0,1)*Pi(2,3) - Pi(0,3)*Pi(2,1);
  double tz  = Pi(0,2)*Pi(2,3) - Pi(0,3)*Pi(2,2);

  Bij(0,0) = ( txy*bj[1] + txz*bj[2] + tx) / denom;
  Bij(0,1) = (-txy*bj[0] + tyz*bj[2] + ty) / denom;
  Bij(0,2) = (-txz*bj[0] - tyz*bj[1] + tz) / denom;

  txy = Pi(1,0)*Pi(2,1) - Pi(1,1)*Pi(2,0);
  txz = Pi(1,0)*Pi(2,2) - Pi(1,2)*Pi(2,0);
  tyz = Pi(1,1)*Pi(2,2) - Pi(1,2)*Pi(2,1);
  tx  = Pi(1,0)*Pi(2,3) - Pi(1,3)*Pi(2,0);
  ty  = Pi(1,1)*Pi(2,3) - Pi(1,3)*Pi(2,1);
  tz  = Pi(1,2)*Pi(2,3) - Pi(1,3)*Pi(2,2);

  Bij(1,0) = ( txy*bj[1] + txz*bj[2] + tx) / denom;
  Bij(1,1) = (-txy*bj[0] + tyz*bj[2] + ty) / denom;
  Bij(1,2) = (-txz*bj[0] - tyz*bj[1] + tz) / denom;
}


//: Fast conversion of rotation from Rodrigues vector to matrix
vnl_matrix_fixed<double,3,3>
bpgl_bundle_rolling_shutter_adj_lsqr::rod_to_matrix(const double* r) const
{
  double x2 = r[0]*r[0], y2 = r[1]*r[1], z2 = r[2]*r[2];
  double m = x2 + y2 + z2;
  double theta = std::sqrt(m);
  double s = std::sin(theta) / theta;
  double c = (1 - std::cos(theta)) / m;

  vnl_matrix_fixed<double,3,3> R(0.0);
  R(0,0) = R(1,1) = R(2,2) = 1.0;
  if (m == 0.0)
    return R;

  R(0,0) -= (y2 + z2) * c;
  R(1,1) -= (x2 + z2) * c;
  R(2,2) -= (x2 + y2) * c;
  R(0,1) = R(1,0) = r[0]*r[1]*c;
  R(0,2) = R(2,0) = r[0]*r[2]*c;
  R(1,2) = R(2,1) = r[1]*r[2]*c;

  double t = r[0]*s;
  R(1,2) -= t;
  R(2,1) += t;
  t = r[1]*s;
  R(0,2) += t;
  R(2,0) -= t;
  t = r[2]*s;
  R(0,1) -= t;
  R(1,0) += t;

  return R;
}

vnl_matrix_fixed<double,3,3>
bpgl_bundle_rolling_shutter_adj_lsqr::vector_to_skewmatrix(const double* r) const
{
  vnl_matrix_fixed<double,3,3> w(0.0);

  w(0,0)=0.0;w(0,1)=-r[2];w(0,2)=r[1];
  w(1,0)=r[2];w(1,1)=0.0;w(1,2)=-r[0];
  w(2,0)=-r[1];w(2,1)=r[0];w(2,2)=0.0;

  return w;
}

//: Create the parameter vector \p a from a vector of cameras
vnl_vector<double>
bpgl_bundle_rolling_shutter_adj_lsqr::create_param_vector(const std::vector<vpgl_perspective_camera<double> >& cameras,
                                                          const std::vector<vnl_vector<double> > & motion)
{
  vnl_vector<double> a(12*cameras.size(),0.0);
  for (unsigned int i=0; i<cameras.size(); ++i)
  {
    const vpgl_perspective_camera<double>& cam = cameras[i];
    const vgl_point_3d<double>& c = cam.get_camera_center();
    const vgl_rotation_3d<double>& R = cam.get_rotation();

    // compute the Rodrigues vector from the rotation
    vnl_vector_fixed<double,3> w = R.as_rodrigues();

    double* ai = a.data_block() + i*12;
    ai[0]=w[0];   ai[1]=w[1];   ai[2]=w[2];
    ai[3]=c.x();  ai[4]=c.y();  ai[5]=c.z();

    for (unsigned j=0;j<6;j++)
      ai[j+6]=motion[i][j];
  }
  return a;
}


//: Create the parameter vector \p b from a vector of 3D points
vnl_vector<double>
bpgl_bundle_rolling_shutter_adj_lsqr::create_param_vector(const std::vector<vgl_point_3d<double> >& world_points)
{
  vnl_vector<double> b(3*world_points.size(),0.0);
  for (unsigned int j=0; j<world_points.size(); ++j) {
    const vgl_point_3d<double>& point = world_points[j];
    double* bj = b.data_block() + j*3;
    bj[0]=point.x();  bj[1]=point.y();  bj[2]=point.z();
  }
  return b;
}

//===============================================================
// Static functions for bpgl_bundle_rolling_shutter_adjust
//===============================================================


bpgl_bundle_rolling_shutter_adjust::bpgl_bundle_rolling_shutter_adjust()
  : ba_func_(nullptr),
    use_weights_(false),
    use_gradient_(true),
    start_error_(0.0),
    end_error_(0.0)
{
}

bpgl_bundle_rolling_shutter_adjust::~bpgl_bundle_rolling_shutter_adjust()
{
  delete ba_func_;
}

//: Bundle Adjust
bool
bpgl_bundle_rolling_shutter_adjust::optimize(std::vector<vpgl_perspective_camera<double> >& cameras,
                                             std::vector<vnl_vector<double> > & motion,
                                             double & r,
                                             std::vector<vgl_point_3d<double> >& world_points,
                                             const std::vector<vgl_point_2d<double> >& image_points,
                                             const std::vector<std::vector<bool> >& mask)
{
  // Extract the camera and point parameters
  std::vector<vpgl_calibration_matrix<double> > K;
  a_ = bpgl_bundle_rolling_shutter_adj_lsqr::create_param_vector(cameras,motion);
  b_ = bpgl_bundle_rolling_shutter_adj_lsqr::create_param_vector(world_points);
  K.reserve(cameras.size());
for (auto & camera : cameras) {
    K.push_back(camera.get_calibration());
  }

  // do the bundle adjustment
  delete ba_func_;
  ba_func_ = new bpgl_bundle_rolling_shutter_adj_lsqr(K,image_points,mask,r,use_weights_);
  vnl_sparse_lm lm(*ba_func_);
  lm.set_trace(true);
  lm.set_verbose(true);
  lm.set_x_tolerance(1e-7);
  vnl_vector<double> dummy(1); // TODO -- this workaround will most likely not function very well...
  if (!lm.minimize(a_,b_,dummy,use_gradient_))
    return false;

  start_error_ = lm.get_start_error();
  end_error_ = lm.get_end_error();
  num_iterations_ = lm.get_num_iterations();

  // Update the camera parameters
  for (unsigned int i=0; i<cameras.size(); ++i)
    cameras[i] = ba_func_->param_to_cam(i,a_);
  for (unsigned int i=0; i<cameras.size(); ++i)
    motion[i]=ba_func_->param_to_motion(i,a_);

  // Update the point locations
  for (unsigned int j=0; j<world_points.size(); ++j)
    world_points[j] = ba_func_->param_to_point(j,b_);

  return true;
}


//: Write cameras and points to a file in VRML 2.0 for debugging
void
bpgl_bundle_rolling_shutter_adjust::write_vrml(const std::string& filename,
                                               std::vector<vpgl_perspective_camera<double> >& cameras,
                                               std::vector<vgl_point_3d<double> >& world_points)
{
  std::ofstream os(filename.c_str());
  os << "#VRML V2.0 utf8\n\n";

  for (unsigned int i=0; i<cameras.size(); ++i) {
    vnl_double_3x3 K = cameras[i].get_calibration().get_matrix();

    const vgl_rotation_3d<double>& R = cameras[i].get_rotation();
    //R.set_row(1,-1.0*R.get_row(1));
    //R.set_row(2,-1.0*R.get_row(2));
    vgl_point_3d<double> ctr = cameras[i].get_camera_center();
    double fov = 2.0*std::max(std::atan(K[1][2]/K[1][1]), std::atan(K[0][2]/K[0][0]));
    os  << "Viewpoint {\n"
        << "  position    "<< ctr.x() << ' ' << ctr.y() << ' ' << ctr.z() << '\n'
        << "  orientation "<< R.axis() << ' '<< R.angle() << '\n'
        << "  fieldOfView "<< fov << '\n'
        << "  description \"Camera" << i << "\"\n}\n";
  }

  os << "Shape {\n  appearance NULL\n    geometry PointSet {\n"
     << "      color Color { color [1 0 0] }\n      coord Coordinate{\n"
     << "       point[\n";

  for (auto & world_point : world_points) {
    os  << world_point.x() << ' '
        << world_point.y() << ' '
        << world_point.z() << '\n';
  }
  os << "   ]\n  }\n }\n}\n";

  os.close();
}
