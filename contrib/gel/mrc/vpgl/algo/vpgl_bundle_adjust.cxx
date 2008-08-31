// This is gel/mrc/vpgl/algo/vpgl_bundle_adjust.cxx
#include "vpgl_bundle_adjust.h"
//:
// \file

#include <vnl/vnl_vector_ref.h>
#include <vnl/vnl_double_3.h>
#include <vnl/algo/vnl_sparse_lm.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_algorithm.h>
#include <vcl_cassert.h>


//: Constructor
vpgl_bundle_adj_lsqr::
  vpgl_bundle_adj_lsqr(const vcl_vector<vpgl_calibration_matrix<double> >& K,
                       const vcl_vector<vgl_point_2d<double> >& image_points,
                       const vcl_vector<vcl_vector<bool> >& mask,
                       bool use_confidence_weights)
 : vnl_sparse_lst_sqr_function(K.size(),6,mask[0].size(),3,mask,2,use_gradient),
   K_(K),
   image_points_(image_points),
   use_covars_(false),
   use_weights_(use_confidence_weights),
   weights_(image_points.size(),1.0),
   iteration_count_(0)
{
  for (unsigned int i=0; i<K_.size(); ++i)
    Km_.push_back(K_[i].get_matrix());
}


//: Constructor
//  Each image point is assigned an inverse covariance (error projector) matrix
// \note image points are not homogeneous because they require finite points to measure projection error
vpgl_bundle_adj_lsqr::
vpgl_bundle_adj_lsqr(const vcl_vector<vpgl_calibration_matrix<double> >& K,
                     const vcl_vector<vgl_point_2d<double> >& image_points,
                     const vcl_vector<vnl_matrix<double> >& inv_covars,
                     const vcl_vector<vcl_vector<bool> >& mask,
                     bool use_confidence_weights)
 : vnl_sparse_lst_sqr_function(K.size(),6,mask[0].size(),3,mask,2,use_gradient),
   K_(K),
   image_points_(image_points),
   use_covars_(true),
   use_weights_(use_confidence_weights),
   weights_(image_points.size(),1.0),
   iteration_count_(0)
{
  for (unsigned int i=0; i<K_.size(); ++i)
    Km_.push_back(K_[i].get_matrix());

  assert(image_points.size() == inv_covars.size());
  vnl_matrix<double> U(2,2,0.0);
  for (unsigned i=0; i<inv_covars.size(); ++i)
  {
    const vnl_matrix<double>& S = inv_covars[i];
    if (S(0,0) > 0.0) {
      U(0,0) = vcl_sqrt(S(0,0));
      U(0,1) = S(0,1)/U(0,0);
      double U11 = S(1,1)-S(0,1)*S(0,1)/S(0,0);
      U(1,1) = (U11>0.0)?vcl_sqrt(U11):0.0;
    }
    else if (S(1,1) > 0.0) {
      assert(S(0,1) == 0.0);
      U(0,0) = 0.0;
      U(0,1) = 0.0;
      U(1,1) = vcl_sqrt(S(1,1));
    }
    else {
      vcl_cout << "warning: not positive definite"<<vcl_endl;
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
vpgl_bundle_adj_lsqr::f(vnl_vector<double> const& a,
                        vnl_vector<double> const& b,
                        vnl_vector<double>& e)
{
  typedef vnl_crs_index::sparse_vector::iterator sv_itr;
  for (unsigned int i=0; i<number_of_a(); ++i)
  {
    //: Construct the ith camera
    vnl_double_3x4 Pi = param_to_cam_matrix(i,a);

    vnl_crs_index::sparse_vector row = residual_indices_.sparse_row(i);
    for (sv_itr r_itr=row.begin(); r_itr!=row.end(); ++r_itr)
    {
      unsigned int j = r_itr->second;
      unsigned int k = r_itr->first;

      // Construct the jth point
      const double * bj = b.data_block()+index_b(j);
      vnl_vector_fixed<double,4> Xj(bj[0], bj[1], bj[2], 1.0);

      // Project jth point with the ith camera
      vnl_vector_fixed<double,3> xij = Pi*Xj;

      double* eij = e.data_block()+index_e(k);
      eij[0] = xij[0]/xij[2] - image_points_[k].x();
      eij[1] = xij[1]/xij[2] - image_points_[k].y();
      if (use_covars_){
        // multiple this error by upper triangular Sij
        vnl_matrix<double>& Sij = factored_inv_covars_[k];
        eij[0] *= Sij(0,0);
        eij[0] += eij[1]*Sij(0,1);
        eij[1] *= Sij(1,1);
      }
    }
  }

  if (use_weights_){
    vnl_vector<double> unweighted(e);
    for (unsigned int k=0; k<weights_.size(); ++k){
      e[2*k]   *= weights_[k];
      e[2*k+1] *= weights_[k];
    }
    // weighted average error
    double avg_error = e.rms();
    for (unsigned int k=0; k<weights_.size(); ++k){
      vnl_vector_ref<double> uw(2,unweighted.data_block()+2*k);
      double update = 2.0*avg_error/uw.rms();
      if (update < 1.0)
        weights_[k] = vcl_min(weights_[k], update);
      else
        weights_[k] = 1.0;
      //vcl_cout << weights_[k] << ' ';
      e[2*k]   = unweighted[2*k]   * weights_[k];
      e[2*k+1] = unweighted[2*k+1] * weights_[k];
    }
    vcl_cout << vcl_endl;
  }
}


//: Compute the residuals from the ith component of a and the jth component of b.
//  This is not normally used because f() has a self-contained efficient implementation
//  It is used for finite-differencing if the gradient is marked as unavailable
void
vpgl_bundle_adj_lsqr::fij(int i, int j, vnl_vector<double> const& ai,
                          vnl_vector<double> const& bj, vnl_vector<double>& fij)
{
  //: Construct the ith camera
  vnl_double_3x4 Pi = param_to_cam_matrix(i,ai.data_block());

  // Construct the jth point
  vnl_vector_fixed<double,4> Xj(bj[0], bj[1], bj[2], 1.0);

  // Project jth point with the ith camera
  vnl_vector_fixed<double,3> xij = Pi*Xj;

  int k = residual_indices_(i,j);
  fij[0] = xij[0]/xij[2] - image_points_[k].x();
  fij[1] = xij[1]/xij[2] - image_points_[k].y();
  if (use_covars_){
    // multiple this error by upper triangular Sij
    vnl_matrix<double>& Sij = factored_inv_covars_[k];
    fij[0] *= Sij(0,0);
    fij[0] += fij[1]*Sij(0,1);
    fij[1] *= Sij(1,1);
  }
  if (use_weights_){
    fij[0] *= weights_[k];
    fij[1] *= weights_[k];
  }
}


//: Compute the sparse Jacobian in block form.
void
vpgl_bundle_adj_lsqr::jac_blocks(vnl_vector<double> const& a, vnl_vector<double> const& b,
                                 vcl_vector<vnl_matrix<double> >& A,
                                 vcl_vector<vnl_matrix<double> >& B)
{
  typedef vnl_crs_index::sparse_vector::iterator sv_itr;
  for (unsigned int i=0; i<number_of_a(); ++i)
  {
    //: Construct the ith camera
    vnl_double_3x4 Pi = param_to_cam_matrix(i,a);

    // This is semi const incorrect - there is no vnl_vector_ref_const
    const vnl_vector_ref<double> ai(number_of_params_a(i),
                                    const_cast<double*>(a.data_block())+index_a(i));

    vnl_crs_index::sparse_vector row = residual_indices_.sparse_row(i);
    for (sv_itr r_itr=row.begin(); r_itr!=row.end(); ++r_itr)
    {
      unsigned int j = r_itr->second;
      unsigned int k = r_itr->first;
      // This is semi const incorrect - there is no vnl_vector_ref_const
      const vnl_vector_ref<double> bj(number_of_params_b(j),
                                      const_cast<double*>(b.data_block())+index_b(j));

      jac_Bij(Pi,bj,B[k]);   // compute Jacobian B_ij
      jac_Aij(Pi,Km_[i],ai,bj,A[k]); // compute Jacobian A_ij
      if (use_covars_){
        const vnl_matrix<double>& Sij = factored_inv_covars_[k];
        A[k] = Sij*A[k];
        B[k] = Sij*B[k];
      }
      if (use_weights_){
        A[k] *= weights_[k];
        B[k] *= weights_[k];
      }
    }
  }
}


//: compute the Jacobian Aij
void
vpgl_bundle_adj_lsqr::jac_Aij(vnl_double_3x4 const& Pi,
                              vnl_double_3x3 const& K,
                              vnl_vector<double> const& ai,
                              vnl_vector<double> const& bj,
                              vnl_matrix<double>& Aij)
{
  // The translation part.
  //=====================
  // compute by swapping the role of the translation and point position
  // then reused the jac_Bij code
  {
    vnl_double_3x4 sPi(Pi);
    vnl_double_3x3 M = sPi.extract(3,3);
    sPi.set_column(3,-(M*bj));
    // This is semi const incorrect - there is no vnl_vector_ref_const
    const vnl_vector_ref<double> t(3,const_cast<double*>(ai.data_block())+3);

    vnl_matrix<double> Aij_sub(2,3);
    jac_Bij(sPi,t,Aij_sub);
    Aij.update(Aij_sub,0,3);
  }

  // The rotation part.
  //==================
  // relative translation vector
  {
    vnl_double_3 t(bj[0]-ai[3], bj[1]-ai[4], bj[2]-ai[5]);

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
      double m = vcl_sqrt(m2);  // Rodrigues magnitude = rotation angle
      double c = vcl_cos(m);
      double s = vcl_sin(m);

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
}


//: compute the Jacobian Bij
void
vpgl_bundle_adj_lsqr::jac_Bij(vnl_double_3x4 const& Pi, vnl_vector<double> const& bj,
                              vnl_matrix<double>& Bij)
{
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
vpgl_bundle_adj_lsqr::rod_to_matrix(const double* r) const
{
  double x2 = r[0]*r[0], y2 = r[1]*r[1], z2 = r[2]*r[2];
  double m = x2 + y2 + z2;
  double theta = vcl_sqrt(m);
  double s = vcl_sin(theta) / theta;
  double c = (1 - vcl_cos(theta)) / m;

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


//: Create the parameter vector \p a from a vector of cameras
vnl_vector<double>
vpgl_bundle_adj_lsqr::create_param_vector(const vcl_vector<vpgl_perspective_camera<double> >& cameras)
{
  vnl_vector<double> a(6*cameras.size(),0.0);
  for (unsigned int i=0; i<cameras.size(); ++i)
  {
    const vpgl_perspective_camera<double>& cam = cameras[i];
    const vgl_point_3d<double>& c = cam.get_camera_center();
    const vgl_rotation_3d<double>& R = cam.get_rotation();

    // compute the Rodrigues vector from the rotation
    vnl_vector<double> w = R.as_rodrigues();

    double* ai = a.data_block() + i*6;
    ai[0]=w[0];   ai[1]=w[1];   ai[2]=w[2];
    ai[3]=c.x();  ai[4]=c.y();  ai[5]=c.z();
  }
  return a;
}


//: Create the parameter vector \p b from a vector of 3D points
vnl_vector<double>
vpgl_bundle_adj_lsqr::create_param_vector(const vcl_vector<vgl_point_3d<double> >& world_points)
{
  vnl_vector<double> b(3*world_points.size(),0.0);
  for (unsigned int j=0; j<world_points.size(); ++j){
    const vgl_point_3d<double>& point = world_points[j];
    double* bj = b.data_block() + j*3;
    bj[0]=point.x();  bj[1]=point.y();  bj[2]=point.z();
  }
  return b;
}


//===============================================================
// Static functions for vpgl_bundle_adjust
//===============================================================


vpgl_bundle_adjust::vpgl_bundle_adjust()
  : ba_func_(NULL),
    use_weights_(false),
    use_gradient_(true),
    start_error_(0.0),
    end_error_(0.0)
{
}

vpgl_bundle_adjust::~vpgl_bundle_adjust()
{
  delete ba_func_;
}

//: Bundle Adjust
bool
vpgl_bundle_adjust::optimize(vcl_vector<vpgl_perspective_camera<double> >& cameras,
                             vcl_vector<vgl_point_3d<double> >& world_points,
                             const vcl_vector<vgl_point_2d<double> >& image_points,
                             const vcl_vector<vcl_vector<bool> >& mask)
{
  // Extract the camera and point parameters
  vcl_vector<vpgl_calibration_matrix<double> > K;
  a_ = vpgl_bundle_adj_lsqr::create_param_vector(cameras);
  b_ = vpgl_bundle_adj_lsqr::create_param_vector(world_points);
  for (unsigned int i=0; i<cameras.size(); ++i){
    K.push_back(cameras[i].get_calibration());
  }

  // do the bundle adjustment
  delete ba_func_;
  ba_func_ = new vpgl_bundle_adj_lsqr(K,image_points,mask,use_weights_);
  vnl_sparse_lm lm(*ba_func_);
  lm.set_trace(true);
  lm.set_verbose(true);
  if (!lm.minimize(a_,b_,use_gradient_))
    return false;

  start_error_ = lm.get_start_error();
  end_error_ = lm.get_end_error();
  num_iterations_ = lm.get_num_iterations();

  // Update the camera parameters
  for (unsigned int i=0; i<cameras.size(); ++i)
    cameras[i] = ba_func_->param_to_cam(i,a_);
  // Update the point locations
  for (unsigned int j=0; j<world_points.size(); ++j)
    world_points[j] = ba_func_->param_to_point(j,b_);

  return true;
}


//: Write cameras and points to a file in VRML 2.0 for debugging
void
vpgl_bundle_adjust::write_vrml(const vcl_string& filename,
                               vcl_vector<vpgl_perspective_camera<double> >& cameras,
                               vcl_vector<vgl_point_3d<double> >& world_points)
{
  vcl_ofstream os(filename.c_str());
  os << "#VRML V2.0 utf8\n\n";

  for (unsigned int i=0; i<cameras.size(); ++i){
    vnl_double_3x3 K = cameras[i].get_calibration().get_matrix();

    const vgl_rotation_3d<double>& R = cameras[i].get_rotation();
    //R.set_row(1,-1.0*R.get_row(1));
    //R.set_row(2,-1.0*R.get_row(2));
    vgl_point_3d<double> ctr = cameras[i].get_camera_center();
    double fov = 2.0*vcl_max(vcl_atan(K[1][2]/K[1][1]), vcl_atan(K[0][2]/K[0][0]));
    os  << "Viewpoint {\n"
        << "  position    "<< ctr.x() << ' ' << ctr.y() << ' ' << ctr.z() << '\n'
        << "  orientation "<< R.axis() << ' '<< R.angle() << '\n'
        << "  fieldOfView "<< fov << '\n'
        << "  description \"Camera" << i << "\"\n}\n";
  }

  os << "Shape {\n  appearance NULL\n    geometry PointSet {\n"
     << "      color Color { color [1 0 0] }\n      coord Coordinate{\n"
     << "       point[\n";

  for (unsigned int j=0; j<world_points.size(); ++j){
    os  << world_points[j].x() << ' '
        << world_points[j].y() << ' '
        << world_points[j].z() << '\n';
  }
  os << "   ]\n  }\n }\n}\n";

  os.close();
}

