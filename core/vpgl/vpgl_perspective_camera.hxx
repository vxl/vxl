// This is core/vpgl/vpgl_perspective_camera.hxx
#ifndef vpgl_perspective_camera_hxx_
#define vpgl_perspective_camera_hxx_
//:
// \file

#include <algorithm>
#include <fstream>
#include <iostream>
#include <utility>
#include "vpgl_perspective_camera.h"

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_homg_plane_3d.h>
#include <vgl/vgl_line_3d_2_points.h>
#include <vgl/algo/vgl_h_matrix_3d.h>
#include <vnl/vnl_det.h>
#include <vnl/algo/vnl_qr.h>
#include <vnl/vnl_matrix_fixed.h>
#if 0
#include <vgl/io/vgl_io_point_3d.h>
#include <vnl/io/vnl_io_matrix_fixed.h>
#include <vnl/io/vnl_io_vector_fixed.h>
#endif
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vnl/vnl_trace.h>

//-------------------------------------------
template <class T>
vpgl_perspective_camera<T>::vpgl_perspective_camera()
{
  R_.set_identity();
  camera_center_.set( (T)0, (T)0, (T)0 );
  recompute_matrix();
}


//-------------------------------------------
template <class T>
vpgl_perspective_camera<T>::vpgl_perspective_camera(
  const vpgl_calibration_matrix<T>& K,
  const vgl_point_3d<T>& camera_center,
  const vgl_rotation_3d<T> R ) :
  K_( K ), camera_center_( camera_center ), R_(std::move( R ))
{
  recompute_matrix();
}

//-------------------------------------------
template <class T>
vpgl_perspective_camera<T>::vpgl_perspective_camera(
  const vpgl_calibration_matrix<T>& K,
  const vgl_rotation_3d<T> R,
  const vgl_vector_3d<T>& t) :
  K_( K ),  R_(std::move( R ))
{
  this->set_translation(t);
  recompute_matrix();
}

//-------------------------------------------
template <class T>
vpgl_perspective_camera<T>::vpgl_perspective_camera( const vpgl_perspective_camera& that)
  : vpgl_proj_camera<T>(that),
  K_(that.K_),
  camera_center_(that.camera_center_),
  R_(that.R_)
{
}

//-------------------------------------------
template <class T>
vpgl_proj_camera<T>* vpgl_perspective_camera<T>::clone(void) const
{
  return new vpgl_perspective_camera<T>(*this);
}

//------------------------------------
template <class T>
vgl_homg_line_3d_2_points<T> vpgl_perspective_camera<T>::backproject(
  const vgl_homg_point_2d<T>& image_point ) const
{
  // First find a point that projects to "image_point".
  vnl_vector_fixed<T,4> vnl_wp = this->svd()->solve(
    vnl_vector_fixed<T,3>( image_point.x(), image_point.y(), image_point.w() ).as_ref() );
  vgl_homg_point_3d<T> wp( vnl_wp[0], vnl_wp[1], vnl_wp[2], vnl_wp[3] );
  // The ray is then defined by that point and the camera center.
  return vgl_homg_line_3d_2_points<T>( vgl_homg_point_3d<T>(camera_center_), wp );
}

//------------------------------------
template <class T>
vgl_line_3d_2_points<T> vpgl_perspective_camera<T>::backproject(
  const vgl_point_2d<T>& image_point ) const
{
  // First find a point in front of the camera that projects to "image_point".
  vnl_vector_fixed<T,4> vnl_wp = this->svd()->solve(
    vnl_vector_fixed<T,3>( image_point.x(), image_point.y(), 1.0 ).as_ref() );
  vgl_homg_point_3d<T> wp_homg( vnl_wp[0], vnl_wp[1], vnl_wp[2], vnl_wp[3] );
  vgl_point_3d<T> wp;
  if ( !wp_homg.ideal() )
    wp.set( wp_homg.x()/wp_homg.w(), wp_homg.y()/wp_homg.w(), wp_homg.z()/wp_homg.w() );
  else
    wp.set( camera_center_.x()+wp_homg.x(),
            camera_center_.y()+wp_homg.y(),
            camera_center_.z()+wp_homg.z() );
  if ( is_behind_camera( vgl_homg_point_3d<T>( wp ) ) )
    wp = camera_center_ + ( camera_center_-wp );
  // The ray is then defined by that point and the camera center.
  return vgl_line_3d_2_points<T>( camera_center_, wp );
}

template <class T>
vgl_ray_3d<T> vpgl_perspective_camera<T>::
backproject_ray( const vgl_point_2d<T>& image_point ) const
{
  vgl_line_3d_2_points<T> l2 = this->backproject(image_point);
  return vgl_ray_3d<T>(l2.point1(), l2.direction());
}

//-------------------------------------------
template <class T>
vgl_vector_3d<T> vpgl_perspective_camera<T>::principal_axis() const
{
  // See H&Z pg 147
  // P = [M|p4] : We do not need to compute det(M) because we enforce
  // det(K)>0 and det(R)=1 in the construction of P. Thus det(M)>0;
  const vnl_matrix_fixed<T,3,4>& P = this->get_matrix();
  return normalized(vgl_vector_3d<T>(P(2,0),P(2,1),P(2,2)));
}

//------------------------------------
template <class T>
bool vpgl_perspective_camera<T>::is_behind_camera(
  const vgl_homg_point_3d<T>& world_point ) const
{
  vgl_homg_plane_3d<T> l = this->principal_plane();
  T dot = world_point.x()*l.a() + world_point.y()*l.b() +
               world_point.z()*l.c() + world_point.w()*l.d();
  if (world_point.w() < (T)0) dot = ((T)(-1))*dot;
  return dot < 0;
}

//-------------------------------------------
template <class T>
void vpgl_perspective_camera<T>::set_calibration( const vpgl_calibration_matrix<T>& K)
{
  K_ = K;
  recompute_matrix();
}

//-------------------------------------------
template <class T>
void vpgl_perspective_camera<T>::set_camera_center(
  const vgl_point_3d<T>& camera_center )
{
  camera_center_ = camera_center;
  recompute_matrix();
}

//-------------------------------------------
template <class T>
void vpgl_perspective_camera<T>::set_translation(const vgl_vector_3d<T>& t)
{
  vgl_rotation_3d<T> Rt = R_.transpose();
  vgl_vector_3d<T> cv = -(Rt * t);
  camera_center_.set(cv.x(), cv.y(), cv.z());
  recompute_matrix();
}

//-------------------------------------------
template <class T>
void vpgl_perspective_camera<T>::set_rotation( const vgl_rotation_3d<T>& R )
{
  R_ = R;
  recompute_matrix();
}

//-------------------------------------------
template <class T>
vgl_vector_3d<T> vpgl_perspective_camera<T>::get_translation() const
{
  vgl_vector_3d<T> c(camera_center_.x(), camera_center_.y(),camera_center_.z());
  vgl_vector_3d<T> temp = R_*c;
  return -temp;
}

//: Rotate the camera about its center such that it looks at the given point
//  The camera should also be rotated about its principal axis such that
//  the vertical image direction is closest to \p up in the world
template <class T>
void vpgl_perspective_camera<T>::look_at(const vgl_homg_point_3d<T>& point,
                                         const vgl_vector_3d<T>& up )
{
  vgl_vector_3d<T> u = normalized(up);
  vgl_vector_3d<T> look = point - camera_center();
  normalize(look);

#if 0
  T dp = dot_product(look, up);
  bool singularity = std::fabs(std::fabs(static_cast<double>(dp))-1.0)<1e-08;
  assert(!singularity);
#endif
  vgl_vector_3d<T> z = look;

  if (std::fabs(dot_product<T>(u,z)-T(1))<1e-5)
  {
    T r[] = { 1, 0, 0,
              0, 1, 0,
              0, 0, 1 };

    vnl_matrix_fixed<T,3,3> R(r);
    set_rotation(vgl_rotation_3d<T>(R));
  }
  else if (std::fabs(dot_product<T>(u,z)-T(-1))<1e-5)
  {
    T r[] = { 1, 0, 0,
            0, 1, 0,
            0, 0, -1 };

    vnl_matrix_fixed<T,3,3> R(r);
    set_rotation(vgl_rotation_3d<T>(R));
  }
  else
  {
    vgl_vector_3d<T> x = cross_product(-u,z);
    vgl_vector_3d<T> y = cross_product(z,x);
    normalize(x);
    normalize(y);
    normalize(z);

    T r[] = { x.x(), x.y(), x.z(),
              y.x(), y.y(), y.z(),
              z.x(), z.y(), z.z() };

    vnl_matrix_fixed<T,3,3> R(r);
    set_rotation(vgl_rotation_3d<T>(R));
  }
}


//-------------------------------------------
template <class T>
void vpgl_perspective_camera<T>::recompute_matrix()
{
   vnl_matrix_fixed<T,3,4> Pnew( (T)0 );

   // Set new projection matrix to [ I | -C ].
   for ( int i = 0; i < 3; i++ )
     Pnew(i,i) = (T)1;
   Pnew(0,3) = -camera_center_.x();
   Pnew(1,3) = -camera_center_.y();
   Pnew(2,3) = -camera_center_.z();

   // Then multiply on left to get KR[ I | -C ].
   this->set_matrix(K_.get_matrix()*R_.as_matrix()*Pnew);
}


//-------------------------------------------
template <class T>
bool vpgl_perspective_decomposition( const vnl_matrix_fixed<T,3,4>& camera_matrix,
                                     vpgl_perspective_camera<T>& p_camera )
{
  // Extract the left sub matrix H from [ H t ] and check that it has rank 3.
  vnl_matrix_fixed<T,3,3> H = camera_matrix.extract(3,3);
  vnl_vector_fixed<T,3> t = camera_matrix.get_column(3);

  T det = vnl_det(H);
  if ( det == (T)0 ) return false;
  // To insure a true rotation (determinant = 1) we must start with a positive
  // determinant H.  This is decomposed into K and R, each with positive determinant.
  if ( det < (T)0 ) {
    H *= (T)-1;
    t *= (T)-1;
  }

  // Now find the RQ decomposition of the sub matrix and use these to find the params.
  // This will involve some trickery as VXL has no RQ decomposition, but does have QR.
  // Define a matrix "flipping" operation by f(A)ij = An-j,n-i i.e. f flips the matrix
  // about its LL-UR diagonal.  One can prove that if f(A) = B*C then A = f(A)*f(B). So
  // we get the RQ decomposition of A by flipping A, then taking the QR decomposition,
  // then flipping both back, noting that the flipped Q and R will remain orthogonal and
  // upper right triangular respectively.
  vnl_matrix_fixed<T,3,3> Hf;
  for ( int i = 0; i < 3; i++ )
    for ( int j = 0; j < 3; j++ )
      Hf(i,j) = H(2-j,2-i);
  vnl_qr<T> QR( Hf.as_ref() );
  vnl_matrix_fixed<T,3,3> q,r,Qf,Rf;
  q = QR.Q();
  r = QR.R();
  for ( int i = 0; i < 3; i++ ) {
    for ( int j = 0; j < 3; j++ ) {
      Qf(i,j) = q(2-j,2-i);
      Rf(i,j) = r(2-j,2-i);
    }
  }

  // We almost have the K and R parameter blocks, but we must be sure that the diagonal
  // entries of K are positive.
  int r0pos = Rf(0,0) > 0 ? 1 : -1;
  int r1pos = Rf(1,1) > 0 ? 1 : -1;
  int r2pos = Rf(2,2) > 0 ? 1 : -1;
  int diag[3] = { r0pos, r1pos, r2pos };
  vnl_matrix_fixed<T,3,3> K1,R1;
  for ( int i = 0; i < 3; i++ ) {
    for ( int j = 0; j < 3; j++ ) {
      K1(i,j) = diag[j]*Rf(i,j);
      R1(i,j) = diag[i]*Qf(i,j);
    }
  }
  K1 = K1/K1(2,2);

  // Now we extract the parameters from the matrices we've computed;
  vpgl_calibration_matrix<T> new_K( K1 );
  p_camera.set_calibration( new_K );

  vnl_qr<T> QRofH(H.as_ref()); // size 3x3
  vnl_vector<T> c1 = -QRofH.solve(t.as_ref());
  vgl_point_3d<T> new_c( c1(0), c1(1), c1(2) );
  p_camera.set_camera_center( new_c );

  p_camera.set_rotation( vgl_rotation_3d<T>(R1) );

  return true;
}


//-------------------------------------------
template <class T>
vpgl_perspective_camera<T> vpgl_align_down(
  const vpgl_perspective_camera<T>& p0,
  const vpgl_perspective_camera<T>& p1 )
{
  vpgl_perspective_camera<T> new_camera;
  new_camera.set_calibration( p0.get_calibration() );
  new_camera.set_rotation( p1.get_rotation()*p0.get_rotation().inverse() );
  vgl_point_3d<T> a0 = p0.get_rotation()*p0.get_camera_center();
  vgl_point_3d<T> a1 = p0.get_rotation()*p1.get_camera_center();
  vgl_point_3d<T> new_camera_center(a1.x() - a0.x(),
                                    a1.y() - a0.y(),
                                    a1.z() - a0.z() );
  new_camera.set_camera_center( new_camera_center );
  return new_camera;
}


//-------------------------------------------
template <class T>
vpgl_perspective_camera<T> vpgl_align_up(
  const vpgl_perspective_camera<T>& p0,
  const vpgl_perspective_camera<T>& p1 )
{
  vpgl_perspective_camera<T> new_camera;
  new_camera.set_calibration( p0.get_calibration() );
  new_camera.set_rotation( p1.get_rotation()*p0.get_rotation() );
  vgl_point_3d<T> a = p0.get_rotation().inverse()*p1.get_camera_center();
  vgl_point_3d<T> new_camera_center( p0.get_camera_center().x() + a.x(),
                                     p0.get_camera_center().y() + a.y(),
                                     p0.get_camera_center().z() + a.z() );
  new_camera.set_camera_center( new_camera_center );
  return new_camera;
}


//Post-multiply this perspective camera with a 3-d Euclidean transformation
// Must check if the transformation is Euclidean, i.e. rotation matrix
// and translation.   Since we can only work with the external interface
// the update due to the postmultiplication is:
//   K' = K
//   R' = R*Re
//   cc' = transpose(Re)(cc - te)
// where Re and te are the rotation matrix and
//  translation vector of the euclidean transform
template <class T> vpgl_perspective_camera<T>
vpgl_perspective_camera<T>::postmultiply( const vpgl_perspective_camera<T>& in_cam, const vgl_h_matrix_3d<T>& euclid_trans)
{
  assert(euclid_trans.is_euclidean());
  const vpgl_calibration_matrix<T>& K = in_cam.get_calibration();
  const vgl_rotation_3d<T>& R = in_cam.get_rotation();
  const vgl_point_3d<T>& cc = in_cam.get_camera_center();
  vgl_rotation_3d<T> Re(euclid_trans.get_upper_3x3());
  vgl_homg_point_3d<T> t = euclid_trans.get_translation();

  //The transformed rotation matrix
  vgl_rotation_3d<T> Rp(R*Re);

  //must have Euclidean quantities to proceed
  assert(!t.ideal());

  //Transform the camera center
  //get the Euclidean components
  vgl_vector_3d<T> te(t.x()/t.w(), t.y()/t.w(), t.z()/t.w());

  //construct the transformed center
  vgl_point_3d<T> ccp = Re.inverse()*(cc-te);

  return vpgl_perspective_camera<T>(K, ccp, Rp);
}

template <class T>
vpgl_perspective_camera<T> vpgl_perspective_camera<T>::
postmultiply(const vpgl_perspective_camera<T>& camera,
             const vgl_rotation_3d<T>& rot, const vgl_vector_3d<T>& trans)
{
  vgl_h_matrix_3d<T> H;
  H.set_identity();
  H.set_rotation_matrix(rot.as_matrix());
  H.set_translation(trans.x(), trans.y(), trans.z());
  return vpgl_perspective_camera<T>::postmultiply(camera, H);
}

// I/O :------------------------------------------------

//: Write vpgl_perspective_camera to stream
template <class Type>
std::ostream&  operator<<(std::ostream& s,
                         vpgl_perspective_camera<Type> const& p)
{
  vnl_matrix_fixed<Type, 3, 3> k = p.get_calibration().get_matrix();
  vgl_rotation_3d<Type> rot = p.get_rotation();
  vnl_matrix_fixed<Type, 3, 3> Rm = rot.as_matrix();
  vgl_vector_3d<Type> t = p.get_translation();
  s << k << '\n'
    << Rm << '\n'
    << t.x() << ' ' << t.y() << ' ' << t.z() << '\n';
  return s ;
}

//: Read camera from stream
template <class Type>
std::istream&  operator >>(std::istream& s,
                          vpgl_perspective_camera<Type>& p)
{
  vnl_matrix_fixed<Type, 3, 3> k, Rm;
  vnl_vector_fixed<Type, 3> tv;
  s >> k;
  s >> Rm;
  s >> tv;
  vpgl_calibration_matrix<Type> K(k);
  vgl_rotation_3d<Type> rot(Rm);
  vgl_vector_3d<Type> t(tv[0], tv[1], tv[2]);
  p.set_calibration(K);
  p.set_rotation(rot);
  p.set_translation(t);
  return s ;
}

//: Save in ascii format
template <class Type>
void vpgl_perspective_camera<Type>::save(std::string cam_path)
{
  std::ofstream os(cam_path.c_str());
  if (!os.is_open()) {
    std::cout << "unable to open output stream in vpgl_proj_camera<T>::save(.)\n";
    return;
  }
  os << *this << '\n';
  os.close();
}

//: Write vpgl_perspective_camera to a vrml file
template <class Type>
void vrml_write(std::ostream& str, vpgl_perspective_camera<Type> const& p, double rad)
{
  vgl_point_3d<Type> cent =  p.get_camera_center();
    str << "Transform {\n"
        << "translation " << cent.x() << ' ' << cent.y() << ' '
        << ' ' << cent.z() << '\n'
        << "children [\n"
        << "Shape {\n"
        << " appearance Appearance{\n"
        << "   material Material\n"
        << "    {\n"
        << "      diffuseColor " << 1 << ' ' << 1.0 << ' ' << 0.0 << '\n'
        << "      transparency " << 0.0 << '\n'
        << "    }\n"
        << "  }\n"
        << " geometry Sphere\n"
        <<   "{\n"
        << "  radius " << rad << '\n'
        <<  "   }\n"
        <<  "  }\n"
        <<  " ]\n"
        << "}\n";
    vgl_vector_3d<Type> r = p.principal_axis();
    std::cout<<"principal axis :" <<r<<std::endl;
    vnl_vector_fixed<Type,3> yaxis(0.0, 1.0, 0.0), pvec(r.x(), r.y(), r.z());
    vgl_rotation_3d<Type> rot(yaxis, pvec);
    vnl_quaternion<Type> q = rot.as_quaternion();

    //vnl_double_3 axis = q.axis();
    vnl_vector_fixed<Type,3> axis = q.axis();
    std::cout<<"quaternion "<<axis<< " angle "<<q.angle()<<"\n\n";
    double ang = q.angle();
    str <<  "Transform {\n"
        << " translation " << cent.x()+6*rad*r.x() << ' ' << cent.y()+6*rad*r.y()
        << ' ' << cent.z()+6*rad*r.z() << '\n'
        << " rotation " << axis[0] << ' ' << axis[1] << ' ' << axis[2] << ' ' <<  ang << '\n'
        << "children [\n"
        << " Shape {\n"
        << " appearance Appearance{\n"
        << "  material Material\n"
        << "   {\n"
        << "     diffuseColor 1 0 0\n"
        << "     transparency 0\n"
        << "    }\n"
        << "  }\n"
        << " geometry Cylinder\n"
        << "{\n"
        << " radius "<<rad/3<<'\n'
        << " height " << 12*rad << '\n'
        << " }\n"
        << " }\n"
        << "]\n"
        << "}\n";
}


//: Return a list of camera's, loaded from the (name sorted) files from the given directory
template <class T>
std::vector<vpgl_perspective_camera<T> > cameras_from_directory(std::string dir, T)
{
  std::vector<vpgl_perspective_camera<T> > camlist;
  if (!vul_file::is_directory(dir.c_str()) ) {
    std::cerr << "cameras_from_directory: " << dir << " is not a directory\n";
    return camlist;
  }

  //get all of the cam and image files, sort them
  std::string camglob=dir+"/*";
  vul_file_iterator file_it(camglob.c_str());
  std::vector<std::string> cam_files;
  while (file_it) {
    std::string camName(file_it());
    cam_files.push_back(camName);
    ++file_it;
  }
  std::sort(cam_files.begin(), cam_files.end());

  //take sorted lists and load from file
  for (auto & cam_file : cam_files)
  {
    std::ifstream ifs(cam_file.c_str());
    vpgl_perspective_camera<T> pcam;
    if (!ifs.is_open()) {
      std::cerr << "Failed to open file " << cam_file << '\n';
    }
    else  {
      ifs >> pcam;
      camlist.push_back(pcam);
    }
  }
  return camlist;
}

template <class T>
double vpgl_persp_cam_distance( const vpgl_perspective_camera<T>& cam1, const vpgl_perspective_camera<T>& cam2)
{
  vgl_vector_3d<T> ray1 = cam1.principal_axis();
  vgl_vector_3d<T> ray2 = cam2.principal_axis();

  vgl_rotation_3d<T> R(ray1, ray2);
  double trace = vnl_trace(R.as_matrix());
  return std::acos((trace-1.0)/2.0);  // dist is theta
}

template <class T>
vgl_vector_3d<T> vpgl_persp_cam_base_line_vector( const vpgl_perspective_camera<T>& cam1, const vpgl_perspective_camera<T>& cam2)
{
  vgl_vector_3d<T> out = cam2.get_camera_center()-cam1.get_camera_center();
  return out;
}

template <class T>
vgl_rotation_3d<T> vpgl_persp_cam_relative_orientation( const vpgl_perspective_camera<T>& cam1, const vpgl_perspective_camera<T>& cam2)
{
  vgl_vector_3d<T> p1 = cam1.principal_axis();
  vgl_vector_3d<T> p2 = cam2.principal_axis();

  vgl_rotation_3d<T> R(p2, p1);
  return R;
}

template <class T>
vgl_frustum_3d<T> frustum(vpgl_perspective_camera<T> const& cam,
                          T d_near, T d_far){

  // normal of top face of the frustum
  vgl_vector_3d<T> norm = -cam.principal_axis();
  // get rays through the image corners
  vpgl_calibration_matrix<T> K = cam.get_calibration();
  vgl_point_2d<T> pp = K.principal_point();
  // image corners
  vgl_point_2d<T> ul(T(0), T(0));
  vgl_point_2d<T> ur(T(2.0*pp.x()), T(0));
  vgl_point_2d<T> lr(T(2.0*pp.x()), T(2.0*pp.y()));
  vgl_point_2d<T> ll(T(0), T(2.0*pp.y()));
  std::vector<vgl_ray_3d<T> > corner_rays;
  corner_rays.push_back(cam.backproject_ray(lr));
  corner_rays.push_back(cam.backproject_ray(ur));
  corner_rays.push_back(cam.backproject_ray(ul));
  corner_rays.push_back(cam.backproject_ray(ll));
  return vgl_frustum_3d<T>(corner_rays, norm, d_near, d_far);
}

// Code for easy instantiation.
#undef vpgl_PERSPECTIVE_CAMERA_INSTANTIATE
#define vpgl_PERSPECTIVE_CAMERA_INSTANTIATE(T) \
template class vpgl_perspective_camera<T >; \
template bool vpgl_perspective_decomposition(const vnl_matrix_fixed<T,3,4>& camera_matrix, \
                                             vpgl_perspective_camera<T >& p_camera ); \
template vpgl_perspective_camera<T > vpgl_align_down(const vpgl_perspective_camera<T >& p0, \
                                                     const vpgl_perspective_camera<T >& p1 ); \
template vpgl_perspective_camera<T > vpgl_align_up(const vpgl_perspective_camera<T >& p0, \
                                                   const vpgl_perspective_camera<T >& p1 ); \
template vpgl_perspective_camera<T > postmultiply(const vpgl_perspective_camera<T >& in_cam, \
                                                  const vgl_h_matrix_3d<T >& euclid_trans); \
template double vpgl_persp_cam_distance(const vpgl_perspective_camera<T >& cam1, \
                                        const vpgl_perspective_camera<T >& cam2); \
template vgl_vector_3d<T> vpgl_persp_cam_base_line_vector(const vpgl_perspective_camera<T>& cam1, \
                                                          const vpgl_perspective_camera<T>& cam2); \
template vgl_rotation_3d<T> vpgl_persp_cam_relative_orientation(const vpgl_perspective_camera<T>& cam1, \
                                                                const vpgl_perspective_camera<T>& cam2); \
template void vrml_write(std::ostream &s, const vpgl_perspective_camera<T >&, double rad); \
template std::vector<vpgl_perspective_camera<T > > cameras_from_directory(std::string dir, T); \
 template vgl_frustum_3d<T> frustum(vpgl_perspective_camera<T> const& cam, T d_near, T d_far); \
template std::ostream& operator<<(std::ostream&, const vpgl_perspective_camera<T >&); \
template std::istream& operator>>(std::istream&, vpgl_perspective_camera<T >&)


#endif // vpgl_perspective_camera_hxx_
