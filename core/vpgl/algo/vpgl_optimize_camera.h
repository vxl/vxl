// This is core/vpgl/algo/vpgl_optimize_camera.h
#ifndef vpgl_optimize_camera_h_
#define vpgl_optimize_camera_h_
//:
// \file
// \brief Methods for projecting geometric structures onto the image
// \author Matt Leotta
// \date March 7, 2005
//
#include <vnl/vnl_least_squares_function.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vpgl/vpgl_perspective_camera.h>

//: this class optimizes the rotation of a perspective camera given an initial estimate and a known internal calibration and position
class vpgl_orientation_lsqr : public vnl_least_squares_function
{
 public:
  //: Constructor
  // \note image points are not homogeneous because require finite points to measure projection error
  vpgl_orientation_lsqr(const vpgl_calibration_matrix<double>& K,
                        const vgl_point_3d<double>& c,
                        const std::vector<vgl_homg_point_3d<double> >& world_points,
                        std::vector<vgl_point_2d<double> >  image_points );
  //: Destructor
  ~vpgl_orientation_lsqr() override = default;

  //: The main function.
  //  Given the parameter vector x, compute the vector of residuals fx.
  //  Fx has been sized appropriately before the call.
  //  The parameters in x are the {wx, wy, wz}
  //  where w is the Rodrigues vector of the rotation.
  void f(vnl_vector<double> const& x, vnl_vector<double>& fx) override;

#if 0
  //: Called after each LM iteration to print debugging etc.
  virtual void trace(int iteration, vnl_vector<double> const& x, vnl_vector<double> const& fx);
#endif

 protected:
  //: The fixed internal camera calibration
  vpgl_calibration_matrix<double> K_;
  //: The fixed camera center
  vgl_point_3d<double> c_;
  //: The known points in the world
  std::vector<vgl_homg_point_3d<double> > world_points_;
  //: The corresponding points in the image
  std::vector<vgl_point_2d<double> > image_points_;
};


//: this class optimizes the rotation/translation of a perspective camera given an initial estimate and a known internal calibration
class vpgl_orientation_position_lsqr : public vnl_least_squares_function
{
 public:
  //: Constructor
  // \note image points are not homogeneous because require finite points to measure projection error
  vpgl_orientation_position_lsqr(const vpgl_calibration_matrix<double>& K,
                                 const std::vector<vgl_homg_point_3d<double> >& world_points,
                                 std::vector<vgl_point_2d<double> >  image_points );
  //: Destructor
  ~vpgl_orientation_position_lsqr() override = default;

  //: The main function.
  //  Given the parameter vector x, compute the vector of residuals fx.
  //  Fx has been sized appropriately before the call.
  //  The parameters in x are really two three component vectors {wx, wy, wz, tx, ty, tz}
  //  where w is the Rodrigues vector of the rotation and t is the translation.
  void f(vnl_vector<double> const& x, vnl_vector<double>& fx) override;

#if 0
  //: Called after each LM iteration to print debugging etc.
  virtual void trace(int iteration, vnl_vector<double> const& x, vnl_vector<double> const& fx);
#endif

 protected:
  //: The fixed internal camera calibration
  vpgl_calibration_matrix<double> K_;
  //: The known points in the world
  std::vector<vgl_homg_point_3d<double> > world_points_;
  //: The corresponding points in the image
  std::vector<vgl_point_2d<double> > image_points_;
};

//: this class optimizes the rotation/translation/calibration of a perspective camera given an initial estimate
class vpgl_orientation_position_calibration_lsqr : public vnl_least_squares_function
{
 public:
  //: Constructor
  // \note image points are not homogeneous because require finite points to measure projection error
  vpgl_orientation_position_calibration_lsqr(const std::vector<vgl_homg_point_3d<double> >& world_points,
                                             std::vector<vgl_point_2d<double> >  image_points );
  //: Destructor
  ~vpgl_orientation_position_calibration_lsqr() override = default;

  //: The main function.
  //  Given the parameter vector x, compute the vector of residuals fx.
  //  Fx has been sized appropriately before the call.
  //  The parameters in x are really two three component vectors {wx, wy, wz, tx, ty, tz}
  //  where w is the Rodrigues vector of the rotation and t is the translation.
  void f(vnl_vector<double> const& x, vnl_vector<double>& fx) override;

#if 0
  //: Called after each LM iteration to print debugging etc.
  virtual void trace(int iteration, vnl_vector<double> const& x, vnl_vector<double> const& fx);
#endif

 protected:
  //: The known points in the world
  std::vector<vgl_homg_point_3d<double> > world_points_;
  //: The corresponding points in the image
  std::vector<vgl_point_2d<double> > image_points_;
};

//: this class optimizes the rotation/translation/focal length of a perspective camera given an initial estimate
class vpgl_orientation_position_focal_lsqr : public vnl_least_squares_function
{
 public:
  //: Constructor
  // \note image points are not homogeneous because require finite points to measure projection error
  vpgl_orientation_position_focal_lsqr(const vpgl_calibration_matrix<double>& K_init,
                                       const std::vector<vgl_homg_point_3d<double> >& world_points,
                                       std::vector<vgl_point_2d<double> >  image_points );
  //: Destructor
  ~vpgl_orientation_position_focal_lsqr() override = default;

  //: The main function.
  //  Given the parameter vector x, compute the vector of residuals fx.
  //  Fx has been sized appropriately before the call.
  //  The parameters in x are really two three component vectors {wx, wy, wz, tx, ty, tz}
  //  where w is the Rodrigues vector of the rotation and t is the translation.
  void f(vnl_vector<double> const& x, vnl_vector<double>& fx) override;

  //: Gradients of the cost-function w.r.t. to the 7 free parameters of x
  //  The seven parameters are rotation(wx, wy, wz), translation(tx, ty, tz) and focal length (Fx)
  void gradf(vnl_vector<double> const&x, vnl_matrix<double>& jacobian) override;

#if 0
  //: Called after each LM iteration to print debugging etc.
  virtual void trace(int iteration, vnl_vector<double> const& x, vnl_vector<double> const& fx);
#endif

 protected:
  //: The initial calibration matrix
  vpgl_calibration_matrix<double> K_init_;
  //: The known points in the world
  std::vector<vgl_homg_point_3d<double> > world_points_;
  //: The corresponding points in the image
  std::vector<vgl_point_2d<double> > image_points_;
};



class vpgl_optimize_camera
{
 public:
  ~vpgl_optimize_camera();

  //: optimize orientation for a perspective camera
  static vpgl_perspective_camera<double>
    opt_orient(const vpgl_perspective_camera<double>& camera,
               const std::vector<vgl_homg_point_3d<double> >& world_points,
               const std::vector<vgl_point_2d<double> >& image_points );

  //: optimize orientation and position for a perspective camera
  static vpgl_perspective_camera<double>
    opt_orient_pos(const vpgl_perspective_camera<double>& camera,
                   const std::vector<vgl_homg_point_3d<double> >& world_points,
                   const std::vector<vgl_point_2d<double> >& image_points );

  //: optimize orientation, position and focal length for a perspective camera
  static vpgl_perspective_camera<double>
    opt_orient_pos_f(const vpgl_perspective_camera<double>& camera,
                     const std::vector<vgl_homg_point_3d<double> >& world_points,
                     const std::vector<vgl_point_2d<double> >& image_points,
                     const double xtol = 0.0001, const unsigned nevals=10000);

  //: optimize orientation, position and internal calibration(no skew)for a perspective camera
  static vpgl_perspective_camera<double>
    opt_orient_pos_cal(const vpgl_perspective_camera<double>& camera,
                       const std::vector<vgl_homg_point_3d<double> >& world_points,
                       const std::vector<vgl_point_2d<double> >& image_points,
                       const double xtol = 0.0001, const unsigned nevals=10000);


 private:
  //: Constructor private - static methods only
  vpgl_optimize_camera();
};

#endif // vpgl_optimize_camera_h_
