#ifndef vimt3d_transform_3d_h_
#define vimt3d_transform_3d_h_
//:
// \file
// \brief A class to define and apply a 3D transformation up to affine.
// \author Graham Vincent, Tim Cootes

#include <vsl/vsl_binary_io.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vnl/io/vnl_io_matrix.h>

//=======================================================================

//: A class to define and apply a 3D transform.
// The transform which can be up to an affine transformation.
// In order of complexity the transform can be
// - Identity     x->x, y->y, z->z
// - Translation  x->x + tx, y->y + ty, z->z + tz
// - ZoomOnly     (Scaling, followed by translation) x->sx.x + tx, etc.
// - RigidBody    (Rotation, followed by translation)
// - Similarity   (Isotropic scaling, followed by rotation, then translation)
// - Affine
//
// One useful special case of Affine involves anisotropic scaling, followed
// by rotation, then translation.
//
// The transform types Translation, ZoomOnly, RigidBody and Similarity have
// a defined order in which scaling, rotation and translation components
// are applied, and the components are thus separable.
// Other transformations (e.g. translation followed by rotation) can be
// obtained by composing multiple transforms. The resulting transform will
// in general be termed affine.
//
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
class vimt3d_transform_3d
{
 public:

  //: Defines form of transformation
  enum Form {Identity,
             Translation,
             ZoomOnly,   //!< Scaling, followed by translation
             RigidBody,  //!< Rotation, followed by translation
             Similarity, //!< Isotropic scaling, followed by rotation, then translation
             Affine};

  //: Construct as identity transform
  vimt3d_transform_3d() :
    xx_(1), xy_(0), xz_(0), xt_(0),
    yx_(0), yy_(1), yz_(0), yt_(0),
    zx_(0), zy_(0), zz_(1), zt_(0),
    tx_(0), ty_(0), tz_(0), tt_(1),
    form_(Identity), inv_uptodate_(false) {}

  // An explicit destructor is required to avoid an internal compiler
  // error in icc 8.0 (internal error: 0_1270)

  //: Destructor
  ~vimt3d_transform_3d() {}

  //: True if identity.
  bool is_identity() const { return form_==Identity; }

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

  //: Sets the transformation to be a translation.
  // \param t_x  Translation in x
  // \param t_y  Translation in y
  // \param t_z  Translation in z
  void set_translation(double t_x, double t_y, double t_z);

  //: Sets the transformation to be anisotropic scaling, followed by translation.
  // The transformation is separable affine.
  // x' = s_x.x + t_x,  y' = s_y.y + t_y,  z' = s_z.z + t_z
  // \param s_x  Scaling in x
  // \param s_y  Scaling in y
  // \param s_z  Scaling in z
  // \param t_x  Translation in x
  // \param t_y  Translation in y
  // \param t_z  Translation in z
  void set_zoom_only( double s_x, double s_y, double s_z,
                      double t_x, double t_y, double t_z);

  //: Sets the transformation to be isotropic scaling, followed by translation.
  // The transformation is separable affine.
  // x' = s.x + t_x,  y' = s.y + t_y,  z' = s.z + t_z
  // \param s  Scaling in x, y and z
  // \param t_x  Translation in x
  // \param t_y  Translation in y
  // \param t_z  Translation in z
  void set_zoom_only(double s, double t_x, double t_y, double t_z)
    { set_zoom_only(s, s, s, t_x, t_y, t_z); }

  //: Sets the transformation to be rotation, followed by translation.
  // The transformation is separable affine.
  // \param r_x  Angle of rotation in x
  // \param r_y  Angle of rotation in y
  // \param r_z  Angle of rotation in z
  // \param t_x  Translation in x
  // \param t_y  Translation in y
  // \param t_z  Translation in z
  void set_rigid_body(double r_x, double r_y, double r_z,
                      double t_x, double t_y, double t_z);

  //: Sets the transformation to be isotropic scaling, followed by rotation, then translation.
  // The transformation is separable affine.
  // \param s  Scaling factor
  // \param r_x  Angle of rotation in x
  // \param r_y  Angle of rotation in y
  // \param r_z  Angle of rotation in z
  // \param t_x  Translation in x
  // \param t_y  Translation in y
  // \param t_z  Translation in z
  void set_similarity(double s,
                      double r_x, double r_y, double r_z,
                      double t_x, double t_y, double t_z);

  //: Sets the transformation to be a special case of Affine: anisotropic scaling, followed by rotation, then translation.
  // \param s_x  Scaling factor in x
  // \param s_y  Scaling factor in y
  // \param s_z  Scaling factor in z
  // \param r_x  Angle of rotation in x
  // \param r_y  Angle of rotation in y
  // \param r_z  Angle of rotation in z
  // \param t_x  Translation in x
  // \param t_y  Translation in y
  // \param t_z  Translation in z
  // \note This creates a special case of Affine. Although this special case
  // is separable, in general Affine transformations are not separable.
  void set_affine(double s_x, double s_y, double s_z,
                  double r_x, double r_y, double r_z,
                  double t_x, double t_y, double t_z);

  //: Sets the transformation to be a special case of Affine.
  // T(x,y,z) = p +x.u +y.v + z.w
  // \param p Origin point
  // \param u Vector to which the x-axis is mapped. The length of \a u indicates scaling in x.
  // \param v Vector to which the y-axis is mapped. The length of \a v indicates scaling in y.
  // \param w Vector to which the z-axis is mapped. The length of \a w indicates scaling in z.
  // \note Currently, the implementation assumes that u,v,w are orthogonal
  // and form a right-handed system. There are asserts for this condition.
  // \note This creates a special case of Affine. Although this special case
  // is separable, in general Affine transformations are not separable.
  void set_affine(const vgl_point_3d<double>& p,
                  const vgl_vector_3d<double>& u,
                  const vgl_vector_3d<double>& v,
                  const vgl_vector_3d<double>& w);

  //: Returns the coordinates of the origin
  vgl_point_3d<double>  origin() const
    { return vgl_point_3d<double> (tt_==1?xt_:xt_/tt_,tt_==1?yt_:yt_/tt_,tt_==1?zt_:zt_/tt_); }

  //: Modifies the transformation so that origin == p.
  // Modifies the transformation so that
  // operator()(vgl_point_3d<double> (0,0)) == p.
  // The rest of the transformation is unaffected.
  // If the transformation was previously the identity,
  // it becomes a translation.
  void set_origin( const vgl_point_3d<double> & );

  //: Applies transformation to (x,y,z)
  // \param x  x coordinate
  // \param y  y co-ord
  // \param z  z co-ord
  //ret: Point = T(x,y,z)
  vgl_point_3d<double>  operator()(double x, double y, double z) const
  {
    switch (form_)
    {
     case Identity :
      return vgl_point_3d<double> (x,y,z);
     case Translation :
      return vgl_point_3d<double> (x+xt_,y+yt_,z+zt_);
     case ZoomOnly :
      return vgl_point_3d<double> (
        x*xx_+xt_,
        y*yy_+yt_,
        z*zz_+zt_);
//   case RigidBody, Similarity, Affine :
     default :
      return vgl_point_3d<double> (
        x*xx_+y*xy_+z*xz_+xt_,
        x*yx_+y*yy_+z*yz_+yt_,
        x*zx_+y*zy_+z*zz_+zt_);
    }
  }

  //: Applies transformation to point p
  // \param p  Point
  // \return Point = T(p)
  vgl_point_3d<double>  operator()(vgl_point_3d<double>  p) const
    { return operator()(p.x(),p.y(),p.z()); }

  //: Returns the inverse of the current transform
  // \return inverse of current transform.
  vimt3d_transform_3d inverse() const;

  //: Returns change in transformed point when original point moved by dp
  // \param p  point
  // \param dp  movement from point
  // \return T(p+dp)-T(p)
  vgl_vector_3d<double>  delta(vgl_point_3d<double>  /*p*/, vgl_vector_3d<double>  dp) const
  {
    switch (form_)
    {
     case Identity :
     case Translation:
      return dp;
     case ZoomOnly :
      return vgl_vector_3d<double> (dp.x()*xx_,
                                    dp.y()*yy_,
                                    dp.z()*zz_);
//   case RigidBody, Similarity, Affine :
     default : // Don't worry that the returned value is independent of p --- this is correct.
      return vgl_vector_3d<double> (
        xx_*(dp.x()*xx_+dp.y()*xy_+dp.z()*xz_),
        yy_*(dp.x()*yx_+dp.y()*yy_+dp.z()*yz_),
        zz_*(dp.x()*zx_+dp.y()*zy_+dp.z()*zz_)
        );
    }
  }

  //: Calculates the product LR
  // \param L  Transform
  // \param R  Transform
  // \return Transform LR = R followed by L
  friend vimt3d_transform_3d operator*(const vimt3d_transform_3d& L,
                                       const vimt3d_transform_3d& R);

  //: Print class to os
  // This function prints the extracted params.
  // \sa params()
  // \sa set()
  void print_summary(vcl_ostream& os) const;

  //: Print class to os
  // This function prints the actual parameters xx_,xy_,xz_,xt_, yx_,yy_,yz_,yt_, zx_,zy_,zz_,zt_, tx_,ty_,tz_,tt_
  void print_all(vcl_ostream& os) const;

  //: Set transformation from stream;
  // You can specify the vector as used in the set() operation.
  // \verbatim
  // form: rigidbody
  // vector: { 0.1 0.1 0.1 2 2 2 }
  // \endverbatim
  // or with explicit parameter names from the set_...() methods.
  // \verbatim
  // form: rigidbody
  // r_x: 0.1
  // r_y: 0.1
  // r_z: 0.1
  // t_x: 2
  // t_y: 2
  // t_z: 2
  // \endverbatim
  void config(vcl_istream& is);

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs);

  //: True if t is the same as this.
  // \note All underlying parameters xx_, xy_, etc are required to be equal,
  // but the declared Form (etc RigidBody) need not be equal.
  bool operator==(const vimt3d_transform_3d&) const;

 protected:

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


//: Calculates the product LR
// \param L  Transform
// \param R  Transform
// \return Transform LR = R followed by L
vimt3d_transform_3d operator*(const vimt3d_transform_3d& L,
                              const vimt3d_transform_3d& R);


//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const vimt3d_transform_3d& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, vimt3d_transform_3d& b);

//: Stream output operator for class reference
vcl_ostream& operator<<(vcl_ostream& os,const vimt3d_transform_3d& b);

//=======================================================================

#endif // vimt3d_transform_3d_h_
