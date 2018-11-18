// This is vpgl/algo/vpgl_ba_fixed_k_lsqr.cxx
#include <utility>
#include "vpgl_ba_fixed_k_lsqr.h"
//:
// \file

#include <vnl/vnl_vector_ref.h>
#include <vgl/algo/vgl_rotation_3d.h>



//: Constructor
vpgl_ba_fixed_k_lsqr::
vpgl_ba_fixed_k_lsqr(std::vector<vpgl_calibration_matrix<double> >  K,
                     const std::vector<vgl_point_2d<double> >& image_points,
                     const std::vector<std::vector<bool> >& mask)
 : vpgl_bundle_adjust_lsqr(6,3,0,image_points,mask),
   K_(std::move(K))
{
  for (auto & i : K_)
    Km_.push_back(i.get_matrix());
}


//: Constructor
//  Each image point is assigned an inverse covariance (error projector) matrix
// \note image points are not homogeneous because they require finite points to measure projection error
vpgl_ba_fixed_k_lsqr::
vpgl_ba_fixed_k_lsqr(std::vector<vpgl_calibration_matrix<double> >  K,
                     const std::vector<vgl_point_2d<double> >& image_points,
                     const std::vector<vnl_matrix<double> >& inv_covars,
                     const std::vector<std::vector<bool> >& mask)
 : vpgl_bundle_adjust_lsqr(6,3,0,image_points,inv_covars,mask),
   K_(std::move(K))
{
  for (auto & i : K_)
    Km_.push_back(i.get_matrix());
}


//: compute the Jacobian Aij
void vpgl_ba_fixed_k_lsqr::jac_Aij(unsigned int i,
                                   unsigned int  /*j*/,
                                   vnl_double_3x4 const& Pi,
                                   vnl_vector<double> const& ai,
                                   vnl_vector<double> const& bj,
                                   vnl_vector<double> const&  /*c*/,
                                   vnl_matrix<double>& Aij)
{
  // the translation part
  // --------------------
  vnl_double_3x3 M = Pi.extract(3,3);
  // This is semi const incorrect - there is no vnl_vector_ref_const
  const vnl_vector_ref<double> C(3,const_cast<double*>(ai.data_block())+3);
  vnl_matrix<double> Aij_sub(2,3);
  jac_camera_center(M,C,bj,Aij_sub);
  Aij.update(Aij_sub,0,3);


  // the rotation part
  // -----------------
  // This is semi const incorrect - there is no vnl_vector_ref_const
  const vnl_vector_ref<double> r(3,const_cast<double*>(ai.data_block()));
  jac_camera_rotation(Km_[i],C,r,bj,Aij);
}

//: compute the Jacobian Bij
void vpgl_ba_fixed_k_lsqr::jac_Bij(unsigned int  /*i*/,
                                   unsigned int  /*j*/,
                                   vnl_double_3x4 const& Pi,
                                   vnl_vector<double> const&  /*ai*/,
                                   vnl_vector<double> const& bj,
                                   vnl_vector<double> const&  /*c*/,
                                   vnl_matrix<double>& Bij)
{
  jac_inhomg_3d_point(Pi, bj, Bij);
}

//: compute the Jacobian Cij
void vpgl_ba_fixed_k_lsqr::jac_Cij(unsigned int  /*i*/,
                                   unsigned int  /*j*/,
                                   vnl_double_3x4 const&  /*Pi*/,
                                   vnl_vector<double> const&  /*ai*/,
                                   vnl_vector<double> const&  /*bj*/,
                                   vnl_vector<double> const&  /*c*/,
                                   vnl_matrix<double>&  /*Cij*/)
{
  // do nothing, c parameters are not used
}

//: construct the \param j-th perspective camera from a pointer to the j-th parameters of \param b and parameters \param c
vgl_homg_point_3d<double>
vpgl_ba_fixed_k_lsqr::param_to_point(int  /*j*/,
                                     const double* bj,
                                     const vnl_vector<double>&  /*c*/) const
{
  return {bj[0], bj[1], bj[2]};
}

//: construct the \param j-th perspective camera from a pointer to the j-th parameters of \param b and parameters \param c
vnl_vector_fixed<double,4>
vpgl_ba_fixed_k_lsqr::param_to_pt_vector(int  /*j*/,
                                         const double* bj,
                                         const vnl_vector<double>&  /*c*/) const
{
  return vnl_vector_fixed<double,4>(bj[0], bj[1], bj[2], 1.0);
}

//: construct the \param i-th perspective camera from a pointer to the i-th parameters of \param a and parameters \param c
vpgl_perspective_camera<double>
vpgl_ba_fixed_k_lsqr::param_to_cam(int i,
                                   const double* ai,
                                   const vnl_vector<double>&  /*c*/) const
{
  vnl_vector<double> w(ai,3);
  vgl_homg_point_3d<double> t(ai[3], ai[4], ai[5]);
  return vpgl_perspective_camera<double>(K_[i],t,vgl_rotation_3d<double>(w));
}

//: compute a 3x4 camera matrix of camera \param i from a pointer to the i-th parameters of \param a and parameters \param c
vnl_double_3x4
vpgl_ba_fixed_k_lsqr::param_to_cam_matrix(int i,
                                          const double* ai,
                                          const vnl_vector<double>&  /*c*/) const
{
  const vnl_vector_ref<double> r(3,const_cast<double*>(ai));
  vnl_double_3x3 M = Km_[i]*rod_to_matrix(r);
  vnl_double_3x4 P;
  P.set_columns(0, M.as_ref());
  const vnl_vector_ref<double> center(3,const_cast<double*>(ai+3));
  P.set_column(3,-(M*center));
  return P;
}


//: Create the parameter vector \p a from a vector of cameras
vnl_vector<double>
vpgl_ba_fixed_k_lsqr::create_param_vector(const std::vector<vpgl_perspective_camera<double> >& cameras)
{
  vnl_vector<double> a(6*cameras.size(),0.0);
  for (unsigned int i=0; i<cameras.size(); ++i)
  {
    const vpgl_perspective_camera<double>& cam = cameras[i];
    const vgl_point_3d<double>& c = cam.get_camera_center();
    const vgl_rotation_3d<double>& R = cam.get_rotation();

    // compute the Rodrigues vector from the rotation
    vnl_vector_fixed<double,3> w = R.as_rodrigues();

    double* ai = a.data_block() + i*6;
    ai[0]=w[0];   ai[1]=w[1];   ai[2]=w[2];
    ai[3]=c.x();  ai[4]=c.y();  ai[5]=c.z();
  }
  return a;
}


//: Create the parameter vector \p b from a vector of 3D points
vnl_vector<double>
vpgl_ba_fixed_k_lsqr::create_param_vector(const std::vector<vgl_point_3d<double> >& world_points)
{
  vnl_vector<double> b(3*world_points.size(),0.0);
  for (unsigned int j=0; j<world_points.size(); ++j){
    const vgl_point_3d<double>& point = world_points[j];
    double* bj = b.data_block() + j*3;
    bj[0]=point.x();  bj[1]=point.y();  bj[2]=point.z();
  }
  return b;
}
