// This is core/vpgl/vpgl_proj_camera.hxx
#ifndef vpgl_proj_camera_hxx_
#define vpgl_proj_camera_hxx_
//:
// \file

#include <iostream>
#include <fstream>
#include "vpgl_proj_camera.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_ray_3d.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/io/vnl_io_matrix_fixed.h>
#include <vgl/vgl_tolerance.h>


// CONSTRUCTORS:--------------------------------------------------------------------

//------------------------------------
template <class T>
vpgl_proj_camera<T>::vpgl_proj_camera() :
  cached_svd_(nullptr)
{
  P_ = vnl_matrix_fixed<T,3,4>( (T)0 );
  P_(0,0) = P_(1,1) = P_(2,2) = (T)1;
}

//------------------------------------
template <class T>
vpgl_proj_camera<T>::vpgl_proj_camera( const vnl_matrix_fixed<T,3,4>& camera_matrix ) :
  P_( camera_matrix ),
  cached_svd_(nullptr)
{
}

//------------------------------------
template <class T>
vpgl_proj_camera<T>::vpgl_proj_camera( const T* camera_matrix ) :
  P_( camera_matrix ),
  cached_svd_(nullptr)
{
}

//------------------------------------
template <class T>
vpgl_proj_camera<T>::vpgl_proj_camera( const vpgl_proj_camera& cam ) :
  vpgl_camera<T>(),
  P_( cam.get_matrix() ),
  cached_svd_(nullptr)
{
}

//------------------------------------
template <class T>
const vpgl_proj_camera<T>& vpgl_proj_camera<T>::operator=( const vpgl_proj_camera& cam )
{
  P_ = cam.get_matrix();
  if ( cached_svd_ != nullptr ) delete cached_svd_;
  cached_svd_ = nullptr;
  return *this;
}

//------------------------------------
template <class T>
vpgl_proj_camera<T>::~vpgl_proj_camera()
{
  if ( cached_svd_ != nullptr ) delete cached_svd_;
  cached_svd_ = nullptr;
}

template <class T>
vpgl_proj_camera<T>* vpgl_proj_camera<T>::clone(void) const
{
  return new vpgl_proj_camera<T>(*this);
}


// PROJECTIONS AND BACKPROJECTIONS:----------------------------------------------

//------------------------------------
template <class T>
vgl_homg_point_2d<T> vpgl_proj_camera<T>::project( const vgl_homg_point_3d<T>& world_point ) const
{
  // For efficiency, manually compute the multiplication rather than converting to
  // vnl and converting back.
  vgl_homg_point_2d<T> image_point(
    P_(0,0)*world_point.x() + P_(0,1)*world_point.y() +
    P_(0,2)*world_point.z() + P_(0,3)*world_point.w(),

    P_(1,0)*world_point.x() + P_(1,1)*world_point.y() +
    P_(1,2)*world_point.z() + P_(1,3)*world_point.w(),

    P_(2,0)*world_point.x() + P_(2,1)*world_point.y() +
    P_(2,2)*world_point.z() + P_(2,3)*world_point.w() );
  return image_point;
}

// -----------------------------------
template <class T>
void
vpgl_proj_camera<T>::project(const T x, const T y, const T z, T& u, T& v) const
{
  vgl_homg_point_3d<T> world_point(x, y, z);
  vgl_homg_point_2d<T> image_point = this->project(world_point);
  if (image_point.ideal(static_cast<T>(1.0e-10)))
  {
    u = 0; v = 0;
    std::cerr << "Warning: projection to ideal image point in vpgl_proj_camera -"
             << " result not valid\n";
    return;
  }
  u = image_point.x()/image_point.w();
  v = image_point.y()/image_point.w();
}

//------------------------------------
template <class T>
vgl_line_segment_2d<T> vpgl_proj_camera<T>::project(
  const vgl_line_segment_3d<T>& world_line ) const
{
  vgl_homg_point_3d<T> point1_w( world_line.point1() );
  vgl_homg_point_3d<T> point2_w( world_line.point2() );
  vgl_point_2d<T> point1_im( project( point1_w ) );
  vgl_point_2d<T> point2_im( project( point2_w ) );
  vgl_line_segment_2d<T> image_line( point1_im, point2_im );
  return image_line;
}

//: Project an infinite line in the world onto an infinite line in the image plane.
template <class T>
vgl_line_2d<T> vpgl_proj_camera<T>::project( const vgl_infinite_line_3d<T>& world_line ) const
{
  vgl_homg_point_3d<T> point1_w( world_line.point() );
  vgl_homg_point_3d<T> point2_w( world_line.point_t(T(1)) );
  vgl_point_2d<T> point1_im( project( point1_w ) );
  vgl_point_2d<T> point2_im( project( point2_w ) );
  vgl_line_2d<T> image_line( point1_im, point2_im );
  return image_line;
}

//------------------------------------
template <class T>
vgl_homg_line_3d_2_points<T> vpgl_proj_camera<T>::backproject(
  const vgl_homg_point_2d<T>& image_point ) const
{
  // First find any point in the world that projects to the "image_point".
  vnl_vector_fixed<T,4> vnl_wp = svd()->solve(
    vnl_vector_fixed<T,3>( image_point.x(), image_point.y(), image_point.w() ).as_ref() );
  vgl_homg_point_3d<T> wp( vnl_wp[0], vnl_wp[1], vnl_wp[2], vnl_wp[3] );

  // The ray is then defined by that point and the camera center.
  if ( wp.ideal(.000001f) ) // modified 11/6/2005 by tpollard
    return vgl_homg_line_3d_2_points<T>( camera_center(), wp );
  return vgl_homg_line_3d_2_points<T>( wp, camera_center() );
}

  //: Find the 3d ray that goes through the camera center and the provided image point.
template <class T>
vgl_ray_3d<T> vpgl_proj_camera<T>::backproject_ray(const vgl_homg_point_2d<T>& image_point ) const
{
  vnl_vector_fixed<T,4> vnl_wp = svd()->solve(
    vnl_vector_fixed<T,3>( image_point.x(), image_point.y(), image_point.w() ).as_ref() );
  vgl_homg_point_3d<T> wp( vnl_wp[0], vnl_wp[1], vnl_wp[2], vnl_wp[3] );
  //in this case the world point defines a direction
  if ( wp.ideal(.000001f) ) {
    vgl_vector_3d<T> dir(wp.x(), wp.y(), wp.z());
    return vgl_ray_3d<T>(this->camera_center(), dir);
  }
  vgl_point_3d<T> wpn(wp);//normalizes
  return vgl_ray_3d<T>(this->camera_center(), wpn);
}

//------------------------------------
template <class T>
vgl_homg_plane_3d<T> vpgl_proj_camera<T>::backproject(
  const vgl_homg_line_2d<T>& image_line ) const
{
  vnl_vector_fixed<T,3> image_line_vnl(image_line.a(),image_line.b(),image_line.c());
  vnl_vector_fixed<T,4> world_plane  = P_.transpose() * image_line_vnl;
  return vgl_homg_plane_3d<T>( world_plane(0), world_plane(1),
                               world_plane(2), world_plane(3) );
}


// MISC CAMERA FUNCTIONS:-----------------------------------------------------

//------------------------------------
template <class T>
vgl_homg_point_3d<T> vpgl_proj_camera<T>::camera_center() const
{
  vnl_matrix<T> ns = svd()->nullspace();
  return vgl_homg_point_3d<T>(ns(0,0), ns(1,0), ns(2,0), ns(3,0));
}

template <class T>
bool vpgl_proj_camera<T>::is_canonical(T tol) const{
  if(tol == T(0)){tol = vgl_tolerance<T>::position;}
  vnl_matrix_fixed<T,3,3> M, I;
  vnl_vector_fixed<T, 3> p;
  this->decompose(M, p);
  bool p_eq_zero =( fabs(p[0])<tol)&&(fabs(p[1])<tol)&&(fabs(p[2])<tol);
  I.set_identity();
  T scale = (fabs(M[0][0]) + fabs(M[1][1]) + fabs(M[2][2]))/T(3);
  if(fabs(scale)<tol)
    return false;
  M/=scale;
  if(M[0][0] < T(0))
    M *= -T(1);
  T id = (M-I).frobenius_norm();
  return p_eq_zero && id < T(10)*tol;
}


// ACCESSORS:-----------------------------------------------------------------

//------------------------------------
template <class T>
vnl_svd<T>* vpgl_proj_camera<T>::svd() const
{
  // Check if the cached copy is valid, if not recompute it.
  if ( cached_svd_ == nullptr )
  {
    cached_svd_ = new vnl_svd<T>(P_.as_ref());

    // Check that the projection matrix isn't degenerate.
    if ( cached_svd_->rank() != 3 )
      std::cerr << "vpgl_proj_camera::svd()\n"
               << "  Warning: Projection matrix is not rank 3, errors may occur.\n";
  }
  return cached_svd_;
}

//------------------------------------
template <class T>
bool vpgl_proj_camera<T>::set_matrix( const vnl_matrix_fixed<T,3,4>& new_camera_matrix )
{
  P_ = new_camera_matrix;
  if ( cached_svd_ != nullptr ) delete cached_svd_;
  cached_svd_ = nullptr;
  return true;
}

//------------------------------------
template <class T>
bool vpgl_proj_camera<T>::set_matrix( const T* new_camera_matrix )
{
  P_ = vnl_matrix_fixed<T,3,4>( new_camera_matrix );
    if ( cached_svd_ != nullptr ) delete cached_svd_;
    cached_svd_ = nullptr;
  return true;
}




// EXTERNAL FUNCTIONS:------------------------------------------------

//: Write vpgl_perspective_camera to stream
template <class Type>
std::ostream&  operator<<(std::ostream& s,
                         vpgl_proj_camera<Type> const& p)
{
  s << "projective:"
    << "\nP\n" << p.get_matrix() << std::endl;

  return s ;
}

//: Read vpgl_perspective_camera from stream
template <class Type>
std::istream&  operator>>(std::istream& s,
                         vpgl_proj_camera<Type>& p)
{
  vnl_matrix_fixed<Type,3,4> new_matrix;
  s >> new_matrix;
  p.set_matrix( new_matrix );

  return s ;
}

template <class T>
void vpgl_proj_camera<T>::save(std::string cam_path)
{
  std::ofstream os(cam_path.c_str());
  if (!os.is_open()) {
    std::cout << "unable to open output stream in vpgl_proj_camera<T>::save(.)\n";
    return;
  }
  os << this->get_matrix() << '\n';
  os.close();
}

//-------------------------------
template <class T>
vgl_h_matrix_3d<T> get_canonical_h( const vpgl_proj_camera<T>& camera )
{
  // If P is a 3x4 rank 3 matrix, Pinv is the pseudo-inverse of P, and l is a
  // vector such that P*l = 0, then P*[Pinv | l] = [I | 0].
  vnl_matrix_fixed<T,4,3> Pinv = camera.svd()->pinverse();
  vnl_vector<T> l = camera.svd()->solve( vnl_vector<T>(3,(T)0) );

  vnl_matrix_fixed<T,4,4> H;
  for ( int i = 0; i < 4; i++ ) {
    for ( int j = 0; j < 3; j++ )
      H(i,j) = Pinv(i,j);
    H(i,3) = l(i);
  }
  return vgl_h_matrix_3d<T>( H );
}

//--------------------------------
template <class T>
void fix_cheirality( vpgl_proj_camera<T>& /*camera*/ )
{
  std::cerr << "fix_cheirality( vpgl_proj_camera<T>& ) not implemented\n";
}

//--------------------------------
template <class T>
void make_canonical( vpgl_proj_camera<T>& camera )
{
  vnl_matrix_fixed<T,3,4> can_cam( (T)0 );
  can_cam(0,0) = can_cam(1,1) = can_cam(2,2) = (T)1;
  camera.set_matrix( can_cam );
}

//--------------------------------
template <class T>
vpgl_proj_camera<T> premultiply( const vpgl_proj_camera<T>& in_camera,
                                 const vnl_matrix_fixed<T,3,3>& transform )
{
  return vpgl_proj_camera<T>( transform*in_camera.get_matrix() );
}

//--------------------------------
template <class T>
vpgl_proj_camera<T> postmultiply( const vpgl_proj_camera<T>& in_camera,
                                  const vnl_matrix_fixed<T,4,4>& transform )
{
  return vpgl_proj_camera<T>( in_camera.get_matrix()*transform );
}

//--------------------------------
template <class T>
vgl_point_3d<T> triangulate_3d_point(const vpgl_proj_camera<T>& c1,
                                     const vgl_point_2d<T>& x1,
                                     const vpgl_proj_camera<T>& c2,
                                     const vgl_point_2d<T>& x2)
{
  vnl_matrix_fixed<T,4,4> A;
  vnl_matrix_fixed<T,3,4> P1 = c1.get_matrix();
  vnl_matrix_fixed<T,3,4> P2 = c2.get_matrix();
  for (int i=0; i<4; i++) {
    A[0][i] = x1.x()*P1[2][i] - P1[0][i];
    A[1][i] = x1.y()*P1[2][i] - P1[1][i];
    A[2][i] = x2.x()*P2[2][i] - P2[0][i];
    A[3][i] = x2.y()*P2[2][i] - P2[1][i];
  }
  vnl_svd<T> svd_solver(A.as_ref());
  vnl_vector_fixed<T, 4> p = svd_solver.nullvector();
  vgl_homg_point_3d<T> hp(p[0],p[1],p[2],p[3]);
  return vgl_point_3d<T>(hp);
}


//: Compute the image projection Jacobians at each point
//  The returned matrices map a differential change in 3D
//  to a differential change in the 2D image at each specified 3D point
template <class T>
std::vector<vnl_matrix_fixed<T,2,3> >
image_jacobians(const vpgl_proj_camera<T>& camera,
                const std::vector<vgl_point_3d<T> >& pts)
{
  const vnl_matrix_fixed<T,3,4>& P = camera.get_matrix();
  vnl_vector_fixed<T,4> denom = P.get_row(2);

  vnl_matrix_fixed<T,3,4> Du;
  Du(0,0) = Du(1,1) = Du(2,2) = 0.0;
  Du(0,1) = P(0,0)*P(2,1) - P(0,1)*P(2,0);
  Du(0,2) = P(0,0)*P(2,2) - P(0,2)*P(2,0);
  Du(1,2) = P(0,1)*P(2,2) - P(0,2)*P(2,1);
  Du(0,3) = P(0,0)*P(2,3) - P(0,3)*P(2,0);
  Du(1,3) = P(0,1)*P(2,3) - P(0,3)*P(2,1);
  Du(2,3) = P(0,2)*P(2,3) - P(0,3)*P(2,2);
  Du(1,0) = -Du(0,1);
  Du(2,0) = -Du(0,2);
  Du(2,1) = -Du(1,2);

  vnl_matrix_fixed<T,3,4> Dv;
  Dv(0,0) = Dv(1,1) = Dv(2,2) = 0.0;
  Dv(0,1) = P(1,0)*P(2,1) - P(1,1)*P(2,0);
  Dv(0,2) = P(1,0)*P(2,2) - P(1,2)*P(2,0);
  Dv(1,2) = P(1,1)*P(2,2) - P(1,2)*P(2,1);
  Dv(0,3) = P(1,0)*P(2,3) - P(1,3)*P(2,0);
  Dv(1,3) = P(1,1)*P(2,3) - P(1,3)*P(2,1);
  Dv(2,3) = P(1,2)*P(2,3) - P(1,3)*P(2,2);
  Dv(1,0) = -Dv(0,1);
  Dv(2,0) = -Dv(0,2);
  Dv(2,1) = -Dv(1,2);


  const std::size_t num_pts = pts.size();
  std::vector<vnl_matrix_fixed<T,2,3> > img_jac(num_pts);

  for (unsigned int i=0; i<num_pts; ++i)
  {
    const vgl_point_3d<T>& pt = pts[i];
    vnl_matrix_fixed<T,2,3>& J = img_jac[i];
    vnl_vector_fixed<T,4>  hpt(pt.x(),pt.y(),pt.z(),1.0);

    T d = dot_product(denom,hpt);
    d *= d;
    J.set_row(0,Du*hpt);
    J.set_row(1,Dv*hpt);
    J /= d;
  }

  return img_jac;
}


// Code for easy instantiation.
#undef vpgl_PROJ_CAMERA_INSTANTIATE
#define vpgl_PROJ_CAMERA_INSTANTIATE(T) \
template class vpgl_proj_camera<T >; \
template vgl_h_matrix_3d<T > get_canonical_h(const vpgl_proj_camera<T >& camera ); \
template void fix_cheirality( vpgl_proj_camera<T >& camera ); \
template void make_canonical( vpgl_proj_camera<T >& camera ); \
template vpgl_proj_camera<T > premultiply( const vpgl_proj_camera<T >& in_camera, \
                                           const vnl_matrix_fixed<T,3,3>& transform ); \
template vpgl_proj_camera<T > premultiply( const vpgl_proj_camera<T >& in_camera, \
                                           const vgl_h_matrix_2d<T >& transform ); \
template vpgl_proj_camera<T > postmultiply(const vpgl_proj_camera<T >& in_camera, \
                                           const vnl_matrix_fixed<T,4,4>& transform ); \
template vpgl_proj_camera<T > postmultiply(const vpgl_proj_camera<T >& in_camera, \
                                           const vgl_h_matrix_3d<T >& transform ); \
template vgl_point_3d<T > triangulate_3d_point(const vpgl_proj_camera<T >& c1, \
                                               const vgl_point_2d<T >& x1, \
                                               const vpgl_proj_camera<T >& c2, \
                                               const vgl_point_2d<T >& x2); \
template std::vector<vnl_matrix_fixed<T,2,3> > \
         image_jacobians(const vpgl_proj_camera<T >& camera, \
                         const std::vector<vgl_point_3d<T > >& pts); \
template std::ostream& operator<<(std::ostream&, const vpgl_proj_camera<T >&); \
template std::istream& operator>>(std::istream&, vpgl_proj_camera<T >&)

#endif // vpgl_proj_camera_hxx_
