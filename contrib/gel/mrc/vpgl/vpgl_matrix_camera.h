

#ifndef VPGL_MATRIX_CAMERA_H
#define VPGL_MATRIX_CAMERA_H
#include <vpgl/vpgl_basic_camera.h>

class vpgl_matrix_camera : public vpgl_basic_camera
{
public:
 
 // Constructors
  vpgl_matrix_camera();
  vpgl_matrix_camera(const vnl_matrix<double>& mat);
 // Destructors
  virtual ~vpgl_matrix_camera(){};

  // virtual interface for camera classes

  //---------------------------------------
  // Different world-to-image methods
  virtual void world_to_image(const vnl_vector<double>&, double& ix, double& iy);
  virtual void world_to_image(const double& x, const double& y, const double& z,
			    double& ix, double& iy);
  //-----------------------------------------
 /* virtual void image_to_world(vnl_vector<double>& ray3d_origin, 
			    vnl_vector<double>& world, double u, double v);

 */
  // A function to set and get the initilization point
 
 
  bool get_3x3matrix(vnl_matrix<double> &mat) const {mat = _matrix; return true;}

  double& operator() (unsigned int r, unsigned int c);

  void set_matrix(const vnl_matrix<double>& mat);
  vnl_matrix<double> & vpgl_matrix_camera::get_matrix();
 //void set_general_matrix(const vnl_matrix<double> & gen);

protected:
  
 vnl_matrix<double> _matrix;
 //vnl_matrix<double> _general;
  
  
};




#endif   // BasicCamera
