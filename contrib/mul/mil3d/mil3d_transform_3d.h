#ifndef mil3d_transform_3d_h_
#define mil3d_transform_3d_h_
//:
// \file
// \brief A 3d transform class
// \author grv
//
// A class to define and apply a 3D transform, which can be up to a
// Euclidean transformation.
// In order of complexity the transform can be
// Identity     x->x, y->y, z->z
// Translation  x->x + tx, y->y + ty, z->z + tz
// ZoomOnly     x->sx.x + tx, y->sy.y + ty, z->sz.z + tz
// RigidBody    (Translate + rotation)
// Euclidean    (Translation + rotation + scale)
// The transformation can be represented by a 4x4 matrix of
// homogeneous co-ordinates.
// \verbatim
// ( xx xy xz xt )
// ( yx yy yz yt )
// ( zx zy zz zt )
// ( tx ty tz tt )
// \endverbatim
// For efficiency the elements are stored explicitly, rather than in a
// vnl_matrix<double>, to avoid lots of copying of matrices with all the
// attendant memory allocation.
//
// \verbatim
//  Modifications
//   11-Feb-2004 - Peter Vanroose - added "Projective" case (implemented inverse)
//   11-Feb-2004 - Peter Vanroose - corrected and extended the implementation of delta()
// \endverbatim

#include <vsl/vsl_binary_io.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vnl/io/vnl_io_matrix.h>
#include <vcl_string.h>


//=======================================================================

//: A class to define and apply a 3D transform.
// The transform which can be up to a Euclidean transformation.
// In order of complexity the transform can be
// Identity     x->x, y->y, z->z
// Translation  x->x + tx, y->y + ty, z->z + tz
// ZoomOnly     x->sx.x + tx, y->sy.y + ty, z->sz.z + tz
// RigidBody    (Translate + rotation)
// Euclidean    (Translation + rotation + scale)
class mil3d_transform_3d
{
 public:

  //: Defines form of transformation
  enum Form { Undefined,
              Identity,
              Translation,
              ZoomOnly,
              RigidBody,
              Similarity,
              Affine,
              Projective};

  //: Constructor
  mil3d_transform_3d() : form_(Undefined), inv_uptodate_(false) { set_identity(); }

  //: Destructor
  virtual ~mil3d_transform_3d() {}

  //: True if identity.
  bool isIdentity() const { return form_==Identity; }

  //: Form of transformation.
  Form form() const { return form_; }

  //: Gets 4x4 Matrix representing transformation
  vnl_matrix<double> matrix() const;

  //: Gets 4x4 Matrix representing transformation
  // \retval M  a 4x4 Matrix representing transformation
  void matrix(vnl_matrix<double>& M) const;

  //: Fills v with parameters
  void params(vnl_vector<double>& v) const;

  //: Sets transform using v
  void set(const vnl_vector<double>& v, Form);

  //: Sets transform to identity.
  void set_identity();

  //: Sets the transformation to be separable affine.
  // x' = s_x.x + t_x,  y' = s_y.y + t_y,  z' = s_z.z + t_z
  // \param s_x  Scaling in x
  // \param s_y  Scaling in y
  // \param s_z  Scaling in z
  // \param t_x  Translation in x
  // \param t_y  Translation in y
  // \param t_z  Translation in z
  void set_zoom_only( double s_x, double s_y, double s_z,
                      double t_x, double t_y, double t_z);

  //: Sets the transformation to be separable affine.
  // x' = s.x + t_x,  y' = s.y + t_y,  z' = s.z + t_z
  // \param s  Scaling in x, y and z
  // \param t_x  Translation in x
  // \param t_y  Translation in y
  // \param t_z  Translation in z
  void set_zoom_only(double s, double t_x, double t_y, double t_z)
    { set_zoom_only(s,s,s,t_x,t_y,t_z); }

  //: Sets the transformation to be a translation.
  // \param t_x  Translation in x
  // \param t_y  Translation in y
  // \param t_z  Translation in z
  void set_translation(double t_x, double t_y, double t_z);

  //: Sets the transformation to rotation then translation.
  // \param r_x  Angle of rotation in x
  // \param r_y  Angle of rotation in y
  // \param r_z  Angle of rotation in z
  // \param t_x  Translation in x
  // \param t_y  Translation in y
  // \param t_z  Translation in z
  void set_rigid_body( double r_x, double r_y, double r_z,
                       double t_x, double t_y, double t_z);

  //: Sets the transformation to apply an overall scaling, rotation then translation.
  // \param s  Scaling factor
  // \param r_x  Angle of rotation in x
  // \param r_y  Angle of rotation in y
  // \param r_z  Angle of rotation in z
  // \param t_x  Translation in x
  // \param t_y  Translation in y
  // \param t_z  Translation in z
  void set_similarity( double s,
                       double r_x, double r_y, double r_z,
                       double t_x, double t_y, double t_z);

  //: Sets the transformation to apply scaling, rotation then translation.
  // \param s_x  Scaling factor in x
  // \param s_y  Scaling factor in y
  // \param s_z  Scaling factor in z
  // \param r_x  Angle of rotation in x
  // \param r_y  Angle of rotation in y
  // \param r_z  Angle of rotation in z
  // \param t_x  Translation in x
  // \param t_y  Translation in y
  // \param t_z  Translation in z
  void set_affine(double s_x, double s_y, double s_z,
                  double r_x, double r_y, double r_z,
                  double t_x, double t_y, double t_z);

  //: Sets the transformation to the given projective transformation matrix
  void set_projective(vnl_matrix<double> const& M);

  //: Returns the coordinates of the origin
  vgl_point_3d<double>  origin() const
    { return vgl_point_3d<double> (tt_==1?xt_:xt_/tt_,tt_==1?yt_:yt_/tt_,tt_==1?zt_:zt_/tt_); }

  //: Modifies the transformation so that origin == p.
  // Modifies the transformation so that
  // operator()(vgl_point_2d<double> (0,0)) == p.
  // The rest of the transformation is unaffected.
  // If the transformation was previously the identity,
  // it becomes a translation.
  void origin( const vgl_point_3d<double> & );

  //: Applies transformation to (x,y,z)
  // \param x  x coordinate
  // \param y  y co-ord
  // \param z  z co-ord
  //ret: Point = T(x,y,z)
  vgl_point_3d<double>  operator()(double x, double y, double z) const;

  //: Applies transformation to point p
  // \param p  Point
  // \return Point = T(p)
  vgl_point_3d<double>  operator()(vgl_point_3d<double>  p) const
    { return operator()(p.x(),p.y(),p.z()); }

  //: Returns the inverse of the current transform
  // \return inverse of current transform.
  mil3d_transform_3d inverse() const;

  //: Returns change in transformed point when original point moved by dp
  // \param p  point
  // \param dp  movement from point
  // \return T(p+dp)-T(p)
  vgl_vector_3d<double>  delta(vgl_point_3d<double>  p, vgl_vector_3d<double>  dp) const;

  //: Calculates the product LR
  // \param L  Transform
  // \param R  Transform
  // \return Transform LR = R followed by L
  friend mil3d_transform_3d operator*(const mil3d_transform_3d& L,
                                      const mil3d_transform_3d& R);

 //: Version number for I/O
 short version_no() const;

 //: Name of the class
 virtual vcl_string is_a() const;

  //: True if this is (or is derived from) class named s
  virtual bool is_class(vcl_string const& s) const;

#if 0
  //: Create a copy on the heap and return base class pointer
  virtual mil3d_transform_3d* clone() const;
#endif

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);

  //: True if t is the same as this
  bool operator==(const mil3d_transform_3d&) const;

 private:

  double xx_,xy_,xz_,xt_,yx_,yy_,yz_,yt_,zx_, zy_, zz_, zt_, tx_,ty_,tz_,tt_;
  Form form_;

  // This is all the inverse data
  // Notice the mutable here - take care if using threads!
  mutable double xx2_,xy2_,xz2_,xt2_,yx2_,yy2_,yz2_,yt2_,zx2_, zy2_, zz2_, zt2_, tx2_,ty2_,tz2_,tt2_;
  mutable bool inv_uptodate_;

  void calcInverse() const;
  void setCheck(int n1,int n2,const char* str) const;
  void angles(double& phi_x, double& phi_y, double& phi_z) const;
  void setRotMat(double r_x, double r_y, double r_z);
};

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mil3d_transform_3d& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mil3d_transform_3d& b);

//: Stream output operator for class reference
vcl_ostream& operator<<(vcl_ostream& os,const mil3d_transform_3d& b);

//=======================================================================

#endif // mil3d_transform_3d_h_
