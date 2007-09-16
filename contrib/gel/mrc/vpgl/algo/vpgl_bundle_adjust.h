// This is gel/mrc/vpgl/algo/vpgl_bundle_adjust.h
#ifndef vpgl_bundle_adjust_h_
#define vpgl_bundle_adjust_h_
//:
// \file
// \brief Bundle adjustment sparse least squares functions
// \author Matt Leotta
// \date 4/18/05

#include <bnl/bnl_sparse_lst_sqr_function.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vnl/vnl_vector.h>
#include <vcl_vector.h>


//: Computes the residuals for bundle adjustment given that the cameras share a fixed internal calibration
class vpgl_bundle_adj_lsqr : public bnl_sparse_lst_sqr_function
{
 public:
  //: Constructor
  // \note image points are not homogeneous because they require finite points to measure projection error
  vpgl_bundle_adj_lsqr(const vcl_vector<vpgl_calibration_matrix<double> >& K,
                       const vcl_vector<vgl_point_2d<double> >& image_points,
                       const vcl_vector<vcl_vector<bool> >& mask,
                       bool use_confidence_weights = true);

  //: Constructor
  //  Each image point is assigned an inverse covariance (error projector) matrix
  // \note image points are not homogeneous because they require finite points to measure projection error
  vpgl_bundle_adj_lsqr(const vcl_vector<vpgl_calibration_matrix<double> >& K,
                       const vcl_vector<vgl_point_2d<double> >& image_points,
                       const vcl_vector<vnl_matrix<double> >& inv_covars,
                       const vcl_vector<vcl_vector<bool> >& mask,
                       bool use_confidence_weights = true);

  // Destructor
  virtual ~vpgl_bundle_adj_lsqr() {}

  //: Compute all the reprojection errors
  //  Given the parameter vectors a and b, compute the vector of residuals e.
  //  e has been sized appropriately before the call.
  //  The parameters in a for each camera are {wx, wy, wz, tx, ty, tz}
  //  where w is the Rodrigues vector of the rotation and t is the translation.
  //  The parameters in b for each 3D point are {px, py, pz}
  //  the non-homogeneous position.
  virtual void f(vnl_vector<double> const& a, vnl_vector<double> const& b,
                 vnl_vector<double>& e);

  //: Compute the sparse Jacobian in block form.
  virtual void jac_blocks(vnl_vector<double> const& a, vnl_vector<double> const& b,
                          vcl_vector<vnl_matrix<double> >& A,
                          vcl_vector<vnl_matrix<double> >& B);

  //: compute the Jacobian Bij
  void jac_Bij(vnl_double_3x4 const& Pi, vnl_vector<double> const& bj,
               vnl_matrix<double>& Bij);


  //: construct the jth 3D point from parameter vector b
  vgl_homg_point_3d<double> param_to_point(int j, const vnl_vector<double>& b) const
  {
    const double* d = b.data_block() + index_b(j);
    return vgl_homg_point_3d<double>(d[0], d[1], d[2]);
  }

  //: construct the ith perspective camera from parameter vector a
  vpgl_perspective_camera<double> param_to_cam(int i, const vnl_vector<double>& a) const
  {
    return param_to_cam(i, a.data_block()+index_a(i));
  }
  vpgl_perspective_camera<double> param_to_cam(int i, const double* d) const
  {
    vnl_vector<double> w(d,3);
    vgl_homg_point_3d<double> t(d[3], d[4], d[5]);
    return vpgl_perspective_camera<double>(K_[i],t,vgl_rotation_3d<double>(w));
  }

  //: Create the parameter vector \p a from a vector of cameras
  static vnl_vector<double>
    create_param_vector(const vcl_vector<vpgl_perspective_camera<double> >& cameras);

  //: Create the parameter vector \p b from a vector of 3D points
  static vnl_vector<double>
    create_param_vector(const vcl_vector<vgl_point_3d<double> >& world_points);


  void reset() { iteration_count_ = 0; for (unsigned int i=0; i<weights_.size(); ++i) weights_[i] = 1.0; }

  //: return the current weights
  vcl_vector<double> weights() const { return weights_; }

 protected:
  //: The fixed internal camera calibration
  vcl_vector<vpgl_calibration_matrix<double> > K_;
  //: The corresponding points in the image
  vcl_vector<vgl_point_2d<double> > image_points_;
  //: The Cholesky factored inverse covariances for each image point
  vcl_vector<vnl_matrix<double> > factored_inv_covars_;
  //: Flag to enable covariance weighted errors
  bool use_covars_;
  //: Flag to enable confidence weighting
  bool use_weights_;
  //: The corresponding points in the image
  vcl_vector<double> weights_;
  int iteration_count_;
};


//: Static functions for bundle adjustment
class vpgl_bundle_adjust
{
 public:
  ~vpgl_bundle_adjust();

  //: Bundle Adjust
  static bool
    optimize(vcl_vector<vpgl_perspective_camera<double> >& cameras,
             vcl_vector<vgl_point_3d<double> >& world_points,
             const vcl_vector<vgl_point_2d<double> >& image_points,
             const vcl_vector<vcl_vector<bool> >& mask );

  //: Write cameras and points to a file in VRML 2.0 for debugging
  static void write_vrml(const vcl_string& filename,
                         vcl_vector<vpgl_perspective_camera<double> >& cameras,
                         vcl_vector<vgl_point_3d<double> >& world_points);

 private:
  //: Constructor private - static methods only
  vpgl_bundle_adjust();
};

#endif // vpgl_bundle_adjust_h_
