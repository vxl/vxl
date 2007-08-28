// This is contrib/gel/mrc/vpgl/algo/vpgl_bundle_adjust.cxx

//:
// \file

#include "vpgl_bundle_adjust.h"
#include <vnl/vnl_math.h>
#include <vnl/vnl_quaternion.h>
#include <vnl/algo/vnl_cholesky.h>
#include <bnl/algo/bnl_sparse_lm.h>

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_algorithm.h>


//: Constructor
vpgl_bundle_adj_lsqr::
  vpgl_bundle_adj_lsqr(const vcl_vector<vpgl_calibration_matrix<double> >& K,
                        const vcl_vector<vgl_point_2d<double> >& image_points,
                        const vcl_vector<vcl_vector<bool> >& mask,
                        bool use_confidence_weights)
 : bnl_sparse_lst_sqr_function(K.size(),6,mask[0].size(),3,mask,2,use_gradient),
   K_(K),
   image_points_(image_points),
   use_covars_(false),
   use_weights_(use_confidence_weights),
   weights_(image_points.size(),1.0),
   iteration_count_(0)
{
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
 : bnl_sparse_lst_sqr_function(K.size(),6,mask[0].size(),3,mask,2,use_gradient),
   K_(K),
   image_points_(image_points),
   use_covars_(true),
   use_weights_(use_confidence_weights),
   weights_(image_points.size(),1.0),
   iteration_count_(0)
{
  assert(image_points.size() == inv_covars.size());
  vnl_matrix<double> U(2,2,0.0);
  for(unsigned i=0; i<inv_covars.size(); ++i){
    const vnl_matrix<double>& S = inv_covars[i];
    if(S(0,0) > 0.0){
      U(0,0) = vcl_sqrt(S(0,0));
      U(0,1) = S(0,1)/U(0,0);
      double U11 = S(1,1)-S(0,1)*S(0,1)/S(0,0);
      U(1,1) = (U11>0.0)?vcl_sqrt(U11):0.0;
    }
    else if(S(1,1) > 0.0){
      assert(S(0,1) == 0.0);
      U(0,0) = 0.0;
      U(0,1) = 0.0;
      U(1,1) = vcl_sqrt(S(1,1));
    }
    else{
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
vpgl_bundle_adj_lsqr::f(vnl_vector<double> const& a, vnl_vector<double> const& b,
                         vnl_vector<double>& e)
{
  typedef bnl_crs_index::sparse_vector::iterator sv_itr;
  for(unsigned int i=0; i<number_of_a(); ++i)
  {
    //: Construct the ith camera
    vpgl_perspective_camera<double> Pi = param_to_cam(i,a);

    bnl_crs_index::sparse_vector row = residual_indices_.sparse_row(i);
    for(sv_itr r_itr=row.begin(); r_itr!=row.end(); ++r_itr)
    {
      unsigned int j = r_itr->second;
      unsigned int k = r_itr->first;

      // Construct the jth point
      vgl_homg_point_3d<double> Xj = param_to_point(j,b);

      // Project jth point with the ith camera
      vgl_homg_point_2d<double> xij = Pi(Xj);

      double* eij = e.data_block()+index_e(k);
      eij[0] = xij.x()/xij.w() - image_points_[k].x();
      eij[1] = xij.y()/xij.w() - image_points_[k].y();
      if(use_covars_){
        // multiple this error by upper triangular Sij
        vnl_matrix<double>& Sij = factored_inv_covars_[k];
        eij[0] *= Sij(0,0);
        eij[0] += eij[1]*Sij(0,1);
        eij[1] *= Sij(1,1);
      }
    }
  }


  if(use_weights_ && iteration_count_++ > 50 ){
    vnl_vector<double> unweighted(e);
    for(unsigned int k=0; k<weights_.size(); ++k){
      e[2*k]   *= weights_[k];
      e[2*k+1] *= weights_[k];
    }
    // weighted average error
    double avg_error = e.rms();
    for(unsigned int k=0; k<weights_.size(); ++k){
      vnl_vector_ref<double> uw(2,unweighted.data_block()+2*k);
      double update = 2.0*avg_error/uw.rms();
      if(update < 1.0)
        weights_[k] = vcl_min(weights_[k], update);
      else
        weights_[k] = 1.0;
      //vcl_cout << weights_[k] << " ";  
      e[2*k]   = unweighted[2*k]   * weights_[k];
      e[2*k+1] = unweighted[2*k+1] * weights_[k];
    }
    vcl_cout << vcl_endl;
  }
}


//: Compute the sparse Jacobian in block form.
void
vpgl_bundle_adj_lsqr::jac_blocks(vnl_vector<double> const& a, vnl_vector<double> const& b,
                                  vcl_vector<vnl_matrix<double> >& A,
                                  vcl_vector<vnl_matrix<double> >& B)
{
  const double stepsize = 0.001;
  typedef bnl_crs_index::sparse_vector::iterator sv_itr;
  for(unsigned int i=0; i<number_of_a(); ++i)
  {
    //: Construct the ith camera
    vnl_double_3x4 Pi = param_to_cam(i,a).get_matrix();

    bnl_crs_index::sparse_vector row = residual_indices_.sparse_row(i);
    for(sv_itr r_itr=row.begin(); r_itr!=row.end(); ++r_itr)
    {
      unsigned int j = r_itr->second;
      unsigned int k = r_itr->first;
      // This is semi const incorrect - there is no vnl_vector_ref_const
      const vnl_vector_ref<double> bj(number_of_params_b(j),
                                      const_cast<double*>(b.data_block())+index_b(j));

      jac_Bij(Pi,bj,B[k]);  // compute Jacobian B_ij
      if(use_covars_)
        B[k] = factored_inv_covars_[k]*B[k];
    }

    // This is semi const incorrect - there is no vnl_vector_ref_const
    const vnl_vector_ref<double> ai(number_of_params_a(i),
                                    const_cast<double*>(a.data_block())+index_a(i));
    vnl_vector<double> tai = ai;
    for (unsigned int ii = 0; ii < 6; ++ii)
    {
      // calculate camera just to the right of ai[ii]
      double tplus = tai[ii] = ai[ii] + stepsize;
      vpgl_perspective_camera<double> Pi_plus = param_to_cam(i,tai.data_block());

      // calculate camera just to the left of ai[ii]
      double tminus = tai[ii] = ai[ii] - stepsize;
      vpgl_perspective_camera<double> Pi_minus = param_to_cam(i,tai.data_block());

      double h = 1.0 / (tplus - tminus);

      bnl_crs_index::sparse_vector row = residual_indices_.sparse_row(i);
      for(sv_itr r_itr=row.begin(); r_itr!=row.end(); ++r_itr)
      {
        unsigned int j = r_itr->second;
        unsigned int k = r_itr->first;

        // Construct the jth point
        const double* bj = b.data_block()+index_b(j);
        vgl_homg_point_3d<double> Xj(bj[0],bj[1],bj[2]);
        // Project jth point with the ith cameras
        vgl_homg_point_2d<double> xij_plus = Pi_plus(Xj);
        vgl_homg_point_2d<double> xij_minus = Pi_minus(Xj);

        vnl_matrix<double>& Aij = A[k];
        Aij(0,ii) = (xij_plus.x()/xij_plus.w() - xij_minus.x()/xij_minus.w()) * h * weights_[k];
        Aij(1,ii) = (xij_plus.y()/xij_plus.w() - xij_minus.y()/xij_minus.w()) * h * weights_[k];
        if(use_covars_){
          // multiple this column of A by upper triangular Sij
          vnl_matrix<double>& Sij = factored_inv_covars_[k];
          Aij(0,ii) *= Sij(0,0);
          Aij(0,ii) += Aij(1,ii)*Sij(0,1);
          Aij(1,ii) *= Sij(1,1);
        }
      }

      // restore tai
      tai[ii] = ai[ii];
    }
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

  Bij(0,0) = (txy*bj[1] + txz*bj[2] + tx) / denom;
  Bij(0,1) = (txy*bj[0] + tyz*bj[2] + ty) / denom;
  Bij(0,2) = (txz*bj[0] + tyz*bj[1] + tz) / denom;

  txy = Pi(1,0)*Pi(2,1) - Pi(1,1)*Pi(2,0);
  txz = Pi(1,0)*Pi(2,2) - Pi(1,2)*Pi(2,0);
  tyz = Pi(1,1)*Pi(2,2) - Pi(1,2)*Pi(2,1);
  tx  = Pi(1,0)*Pi(2,3) - Pi(1,3)*Pi(2,0);
  ty  = Pi(1,1)*Pi(2,3) - Pi(1,3)*Pi(2,1);
  tz  = Pi(1,2)*Pi(2,3) - Pi(1,3)*Pi(2,2);

  Bij(1,0) = (txy*bj[1] + txz*bj[2] + tx) / denom;
  Bij(1,1) = (txy*bj[0] + tyz*bj[2] + ty) / denom;
  Bij(1,2) = (txz*bj[0] + tyz*bj[1] + tz) / denom;
}



//: Create the parameter vector \p a from a vector of cameras
vnl_vector<double> 
vpgl_bundle_adj_lsqr::create_param_vector(const vcl_vector<vpgl_perspective_camera<double> >& cameras)
{
  vnl_vector<double> a(6*cameras.size(),0.0);
  for(unsigned int i=0; i<cameras.size(); ++i){
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
  for(unsigned int j=0; j<world_points.size(); ++j){
    const vgl_point_3d<double>& point = world_points[j];
    double* bj = b.data_block() + j*3;
    bj[0]=point.x();  bj[1]=point.y();  bj[2]=point.z();
  }
  return b;
}


//===============================================================
// Static functions for vpgl_bundle_adjust
//===============================================================


//: Bundle Adjust
bool
vpgl_bundle_adjust::optimize(vcl_vector<vpgl_perspective_camera<double> >& cameras,
                              vcl_vector<vgl_point_3d<double> >& world_points,
                              const vcl_vector<vgl_point_2d<double> >& image_points,
                              const vcl_vector<vcl_vector<bool> >& mask)
{
  // Extract the camera and point parameters
  vcl_vector<vpgl_calibration_matrix<double> > K;
  vnl_vector<double> a = vpgl_bundle_adj_lsqr::create_param_vector(cameras);
  vnl_vector<double> b = vpgl_bundle_adj_lsqr::create_param_vector(world_points);
  for(unsigned int i=0; i<cameras.size(); ++i){
    K.push_back(cameras[i].get_calibration());
  }

  // do the bundle adjustment
  vpgl_bundle_adj_lsqr ba_func(K,image_points,mask);
  bnl_sparse_lm lm(ba_func);
  lm.set_trace(true);
  lm.set_verbose(true);
  if(!lm.minimize(a,b))
    return false;

  // Update the camera parameters
  for(unsigned int i=0; i<cameras.size(); ++i)
    cameras[i] = ba_func.param_to_cam(i,a);
  // Update the point locations
  for(unsigned int j=0; j<world_points.size(); ++j)
    world_points[j] = ba_func.param_to_point(j,b);
  
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
  
  for(unsigned int i=0; i<cameras.size(); ++i){
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
        << "  description \"Camera" << i << "\"\n"
        << "}\n";
  }
  
  os << "Shape {\n  appearance NULL\n    geometry PointSet {\n"
     << "      color Color { color [1 0 0] }\n      coord Coordinate{\n"
     << "       point[\n";
 
  for(unsigned int j=0; j<world_points.size(); ++j){
    os  << world_points[j].x() << " "
        << world_points[j].y() << " "
        << world_points[j].z() << "\n";
  }
  os << "   ]\n  }\n }\n}\n";

  os.close();
}



