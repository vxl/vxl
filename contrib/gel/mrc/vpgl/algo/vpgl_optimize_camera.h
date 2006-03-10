// This is gel/mrc/vpgl/algo/vpgl_optimize_camera.h
#ifndef _vpgl_optimize_camera_h_
#define _vpgl_optimize_camera_h_
//:
// \file
// \brief Methods for projecting geometric structures onto the image
// \author Matt Leotta
// \date 3/7/05
//

#include <vnl/vnl_least_squares_function.h>
#include <vpgl/vpgl_perspective_camera.h>


//: this class optimizes the rotation of a perspective camera
//  given an initial estimate and a known internal calibration and position
class vpgl_orientation_lsqr : public vnl_least_squares_function
{
 public:
  //: Constructor
  // \note image points are not homogeneous because require finite points to measure projection error
  vpgl_orientation_lsqr(const vpgl_calibration_matrix<double>& K,
                        const vgl_point_3d<double>& c,
                        const vcl_vector<vgl_homg_point_3d<double> >& world_points,
                        const vcl_vector<vgl_point_2d<double> >& image_points );
  //: Destructor
  virtual ~vpgl_orientation_lsqr() {}

  //: The main function.
  //  Given the parameter vector x, compute the vector of residuals fx.
  //  Fx has been sized appropriately before the call.
  //  The parameters in x are the {wx, wy, wz}
  //  where w is the Rodrigues vector of the rotation.
  virtual void f(vnl_vector<double> const& x, vnl_vector<double>& fx);

  //: Called after each LM iteration to print debugging etc.
  //virtual void trace(int iteration, vnl_vector<double> const& x, vnl_vector<double> const& fx);

 protected:
  //: The fixed internal camera calibration
  vpgl_calibration_matrix<double> K_;
  //: The fixed camera center
  vgl_point_3d<double> c_;
  //: The known points in the world
  vcl_vector<vgl_homg_point_3d<double> > world_points_;
  //: The corresponding points in the image
  vcl_vector<vgl_point_2d<double> > image_points_;

};


//: this class optimizes the rotation and translation of a perspective camera
//  given an initial estimate and a known internal calibration
class vpgl_orientation_position_lsqr : public vnl_least_squares_function
{
 public:
  //: Constructor
  // \note image points are not homogeneous because require finite points to measure projection error
  vpgl_orientation_position_lsqr(const vpgl_calibration_matrix<double>& K,
                                 const vcl_vector<vgl_homg_point_3d<double> >& world_points,
                                 const vcl_vector<vgl_point_2d<double> >& image_points );
  //: Destructor
  virtual ~vpgl_orientation_position_lsqr() {}

  //: The main function.
  //  Given the parameter vector x, compute the vector of residuals fx.
  //  Fx has been sized appropriately before the call.
  //  The parameters in x are really two three component vectors {wx, wy, wz, tx, ty, tz}
  //  where w is the Rodrigues vector of the rotation and t is the translation.
  virtual void f(vnl_vector<double> const& x, vnl_vector<double>& fx);

  //: Called after each LM iteration to print debugging etc.
  //virtual void trace(int iteration, vnl_vector<double> const& x, vnl_vector<double> const& fx);

 protected:
  //: The fixed internal camera calibration
  vpgl_calibration_matrix<double> K_;
  //: The known points in the world
  vcl_vector<vgl_homg_point_3d<double> > world_points_;
  //: The corresponding points in the image
  vcl_vector<vgl_point_2d<double> > image_points_;

};


class vpgl_optimize_camera
{

 public:
  ~vpgl_optimize_camera();

  //: optimize orientation for a perspective camera 
  static vpgl_perspective_camera<double>
    opt_orient(const vpgl_perspective_camera<double>& camera,
               const vcl_vector<vgl_homg_point_3d<double> >& world_points,
               const vcl_vector<vgl_point_2d<double> >& image_points );

  //: optimize orientation and position for a perspective camera
  static vpgl_perspective_camera<double>
    opt_orient_pos(const vpgl_perspective_camera<double>& camera,
                   const vcl_vector<vgl_homg_point_3d<double> >& world_points,
                   const vcl_vector<vgl_point_2d<double> >& image_points ); 

 private:
  //: Constructor private - static methods only
  vpgl_optimize_camera();
};

#endif // _vpgl_optimize_camera_h_
