// This is gel/mrc/vpgl/vpgl_matrix_camera.cxx
#include "vpgl_matrix_camera.h"
//:
// \file

#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_math.h> // for vnl_huge_val()

vpgl_matrix_camera::vpgl_matrix_camera():
  _matrix(3,3)
{
  int r, c;
  for (r=0; r<3; r++)
    for (c=0; c<3; c++)
      _matrix(r, c) = (r==c)?1.0:0.0;
}


vpgl_matrix_camera::vpgl_matrix_camera(const vnl_matrix<double>& mat):
  _matrix(3,3)
{
  if ((mat.rows() != 3) || (mat.columns() != 3))
    {
    vcl_cerr << "vpgl_matrix_camera: 3x3 camera matrix not provided. "
             << "Creating identity camera.\n" ;
    int r, c;
    for (r=0; r<3; r++)
      for (c=0; c<3; c++)
        _matrix(r, c) = (r==c)?1.0:0.0;
    }
  else
    _matrix = mat;
}


void vpgl_matrix_camera::world_to_image(vnl_vector<double> const& vect3d,
                                        double &imgu, double &imgv, double)
{
  double dummy=0;
  // Compute the homogeneous image point
  vnl_vector<double> homog_image_pt = _matrix*vect3d;

  // Initialize for default, ie. homogenizing var in image pt is 0.
  imgu = vnl_huge_val(dummy);
  imgv = vnl_huge_val(dummy);

  // Set the values, if the homog var is non-zero
  if (homog_image_pt[2] != 0)
  {
    imgu = homog_image_pt[0]/homog_image_pt[2];
    imgv = homog_image_pt[1]/homog_image_pt[2];
  }
}


//------------------------------------------------------------------------------
//: projects a 3D point to the image plane with this camera.

void vpgl_matrix_camera::world_to_image(double x, double y, double z,
                                        double& ix, double& iy, double time)
{
  vnl_vector<double> vect3d(3); vect3d[0]=x; vect3d[1]=y; vect3d[2]=z;
  this->world_to_image(vect3d, ix, iy, time);
}

#if 0 // image_to_world() commented out
//-----------------------------------------------------------
//: the virtual image to world projection.
//  This implementation uses two facts: 1) the camera center is given by the null vector
//    of the projection matrix.  That is, 0 = T*C. Where T is the 3x4
//    projection matrix and C is the 3-d homegenous position vector, O
//    is a 3 elment column vector of all zeros.
//    2) The ray through any image point can be found by replacing
//    the translation column of the 3x4 projection matrix by the negative
//    of the image point in 2-d homogeneous coordinates.  That is,
//
//    wu    t00 x + t01 y + t02 z + t03
//    wv  = t10 x + t11 y + t12 z + t13
//    w     t20 x + t21 y + t22 z + t23
//
//   It can be seen that as w->infinity the constant terms, ti3, can be
//   ignored. Thus,
//
//    t00 x + t01 y + t02 z - w u    0
//    t10 x + t11 y + t12 z - w v  = 0
//    t20 x + t21 y + t22 z - w      0
//
//    If we define the ray corrdinates as r = (x/w y/w z/w),
//    then r is the nullvector of the SVD of the modified projection matrix.
//    One final consideration is to make sure that the sense of the
//    ray is along the principal ray which is the lower left three elements
//    of T.
void vpgl_matrix_camera::image_to_world(
                                  vnl_vector<double>& pos,
                                  vnl_vector<double>& wray,
                                  double x, double y)
{
  //Get the center of projection

    vnl_svd svd(_general);
    vnl_vector<double> cproj = svd.nullvector();
    for (int i = 0; i<3; i++)
      pos[i] = cproj[i]/cproj[3];

    //Modify the projection matrix according to the supplied image location
    vnl_matrix<double> P3X4(3,4,0.0);
    for (int r = 0; r<3; r++)
      for (int c = 0; c<3; c++)
        P3X4.put(r,c, _general.get(r,c));
    P3X4.put(0,3, -x);   P3X4.put(1,3,-y); P3X4.put(2,3,-1.0);
    //Get the ray as the nullvector of the modified projection matrix
    vnl_svd svdP(P3X4);
    vnl_vector<double> nvec = svdP.nullvector();
    vnl_vector<double> ray(3,0), principal_ray(3,0);
    ray[0]= nvec[0];
    ray[1]= nvec[1];
    ray[2]= nvec[2];
    //Define the principal ray
    principal_ray[0]=_general(2,0);
    principal_ray[1]=_general(2,1);
    principal_ray[2]=_general(2,2);
    //Adjust the sense of the world ray
    double d = dot_product(ray, principal_ray);
    ray /=d;
    for (int i = 0; i<3; i++)
      wray[i] = ray[i];
}
#endif

//: Function to access and set camera parameters.  Performs range check.
double& vpgl_matrix_camera::operator() (unsigned int r, unsigned int c)
{
  // Do the range check
  if ((r > 2) || (c > 3))
  {
    vcl_cerr << "vpgl_matrix_camera:  Parameter indices out of bound. "
             << "Program may segfault now.\n";
  }

  // Return the matrix entry
  return _matrix(r, c);
}

#if 0
//: Function to set the matrix.  Does a dimension check before assigning.
void vpgl_matrix_camera::set_general_matrix(const vnl_matrix<double> & gen){
  _general=gen;
}
#endif

void vpgl_matrix_camera::set_matrix(const vnl_matrix<double>& mat)
{
  if ((mat.rows() != 3) || (mat.columns() != 4))
    vcl_cerr << "vpgl_matrix_camera: Wrong dimensions of the matrix\n";
  else
    _matrix = mat;
}
void vpgl_matrix_camera::get_matrix(vnl_matrix<double> & m) const {
  m = _matrix;
}

// NOTE:  FOLLOWING WILL EVENTUALLY BE MOVED TO THE SOLVER/TARGET INTERFACE
#if 0
//: As above by with matrix represented as double** (for carmen)
// Assumes row/column indexes in mat start from 1, not 0.
//   Removed const because carmen has problems with it... WAH
vpgl_matrix_camera::vpgl_matrix_camera( double **mat):
  _matrix(3,3),_general(3,4)
{
  // Fill up the actual matrix
  for (int i=0;i<3;i++)
    for (int j=0;j<4;j++)
      _general(i,j)=mat[i+1][j+1];
}
#endif

