// This is mul/vimt/vimt_transform_2d.h
#ifndef vimt_transform_2d_h_
#define vimt_transform_2d_h_
//:
// \file
// \author Tim Cootes
// \brief 2D transform, which can be up to a projective transformation
// \verbatim
//  Modifications
//   2011-06-10 Peter Vanroose - modified signature of set_*() to return *this
// \endverbatim

#include <iostream>
#include <iosfwd>
#include <vnl/vnl_fwd.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: 2D transform, which can be up to a projective transformation.
// In order of complexity the transform can be
// \verbatim
// Identity     x->x, y->y
// Translation  x->x + tx, y->y + ty
// ZoomOnly     x->sx.x + tx, y->sy.y + ty
// RigidBody    (Translate + rotation)
// Euclidean    (Translation + rotation + scale)
// Affine
// Projective
// \endverbatim
//
// NOTES:
// The transformation can be represented by a 3x3 matrix mapping homogeneous co-ordinates about.
// \verbatim
// ( xx xy xt )
// ( yx yy yt )
// ( tx ty tt )
// \endverbatim
// For efficiency the elements are stored explicitly, rather than in a vnl_matrix<double>, to avoid lots
// of copying of matrices with all the attendant memory allocation.
//
//
//  Example:
// \code
// vimt_transform_2d T1;
// vimt_transform_2d T2;
// T1.set_zoom(scale,translation.x(),translation.y());
// T2.set_similarity(scale2,theta,translation2.x(),translation2.y());
//
// vimt_transform_2d T3 = T2 * T1; // T1 followed by T2
//
// vgl_point_2d<double>  p(10,10);
// vgl_point_2d<double>  p_new = T3(p);
//
// vimt_transform_2d T_inverse = T3.inverse();
// \endcode
class vimt_transform_2d
{
 public:
    //: Defines form of transformation
    enum Form { Identity,
                Translation,
                ZoomOnly,
                RigidBody,
                Similarity,
                Affine,
                Projective,
                Reflection};

    vimt_transform_2d() :
        xx_(1),xy_(0),xt_(0),
        yx_(0),yy_(1),yt_(0),
        tx_(0),ty_(0),tt_(1),
        form_(Identity),inv_uptodate_(false) {}


    bool is_identity() const { return form_==Identity; }
    Form form() const { return form_; }
    vnl_matrix<double> matrix() const;
    void matrix(vnl_matrix<double>&) const;

    //: Define the transform in terms of a 3x3 homogeneous matrix.
    // \param M 3x3 homogeneous matrix defining the transform.
    // \note Client must ensure that \a M is a valid representation of an affine (or simpler) transform.
    // \note The form will be set to Affine - call simplify() if you need the simplest form.
    vimt_transform_2d& set_matrix(const vnl_matrix<double>& M);

    //: Fills v with parameters
    void params(vnl_vector<double>& v) const { params_of(v,form_); }
    //: Fills v with parameters of transform of type Form
    void params_of(vnl_vector<double>& v, Form) const;
    //: Sets transform using v (converse of params(v))
    vimt_transform_2d& set(const vnl_vector<double>& v, Form); // Sets transform using v
    //: Set to identity transformation
    vimt_transform_2d& set_identity();
    //: Sets the transformation to be separable affine.
    // x' = s_x.x + t_x,  y' = s_y.y + t_y
    // s_x: Scaling in x
    // s_y: Scaling in y
    // t_x: Translation in x
    // t_y: Translation in y
    vimt_transform_2d& set_zoom_only(double s_x, double s_y, double t_x, double t_y);
    //: Sets the transformation to be a zoom.
    // x' = s.x + t_x,  y' = s.y + t_y
    //   s: Scaling
    // t_x: Translation in x
    // t_y: Translation in y
    vimt_transform_2d& set_zoom_only(double s, double t_x, double t_y) { return set_zoom_only(s,s,t_x,t_y); }
    //: Sets the transformation to be separable affine.
    // x' = s_x.x + xt_,  y' = s_y.y + yt_
    // s_x: Scaling in x
    // s_y: Scaling in y
    // Translation in x and y kept as it was
    vimt_transform_2d& set_zoom_only(double s_x, double s_y) { return set_zoom_only(s_x,s_y,xt_,yt_); }
    //: Sets the transformation to be a zoom.
    // x' = s.x + xt_,  y' = s.y + yt_
    //   s: Scaling
    // Translation in x and y kept as it was
    vimt_transform_2d& set_zoom_only(double s) { return set_zoom_only(s,s,xt_,yt_); }
    //: Sets the transformation to be a translation.
    // t_x: Translation in x
    // t_y: Translation in y
    vimt_transform_2d& set_translation(double t_x, double t_y);
    //: Sets the transformation to rotation then translation.
    // theta: rotation
    // t_x: Translation in x
    // t_y: Translation in y
    vimt_transform_2d& set_rigid_body(double theta, double t_x, double t_y);
    //: Sets the transformation to apply scaling, rotation then translation.
    // s: Scaling
    // theta: rotation
    // t_x: Translation in x
    // t_y: Translation in y
    vimt_transform_2d& set_similarity(double s, double theta, double t_x, double t_y);
    //: Sets Euclidean transformation.
    // \param dx  Rotation and scaling of x axis
    // \param t  Translation
    vimt_transform_2d& set_similarity(const vgl_point_2d<double> & dx,
                                      const vgl_point_2d<double> & t);
    //: Sets Euclidean transformation.
    // \param dx  Rotation and scaling of x axis
    // \param t  Translation
    vimt_transform_2d& set_similarity(const vgl_vector_2d<double> & dx,
                                      const vgl_point_2d<double> & t);
    //: reflect about a line though the points m1, and m2
    vimt_transform_2d& set_reflection( const vgl_point_2d<double> & m1,
                                       const vgl_point_2d<double> & m2);
    //: Sets to be 2D affine transformation using 2x3 matrix
    vimt_transform_2d& set_affine(const vnl_matrix<double>&);
    //: Sets to be 2D affine transformation T(x,y)=p+x.u+y.v
    vimt_transform_2d& set_affine(const vgl_point_2d<double> & p,
                                  const vgl_vector_2d<double> & u,
                                  const vgl_vector_2d<double> & v);
    //: Sets to be 2D projective transformation
    vimt_transform_2d& set_projective(const vnl_matrix<double>&);   // 3x3 matrix

    //: Returns the coordinates of the origin.
    // I.e. operator()(vgl_point_2d<double> (0,0))
    vgl_point_2d<double>  origin() const { return {xt_/tt_,yt_/tt_}; }

    //: Modifies the transformation so that operator()(vgl_point_2d<double> (0,0)) == p.
    // The rest of the transformation is unaffected.
    // If the transformation was previously the identity,
    // it becomes a translation.
    vimt_transform_2d& set_origin( const vgl_point_2d<double> & );

    //: Applies transformation to (x,y)
    vgl_point_2d<double>  operator()(double x, double y) const;
    //: Returns transformation applied to point p
    vgl_point_2d<double>  operator()(const vgl_point_2d<double> & p) const { return operator()(p.x(),p.y()); }

    //: Calculates inverse of this transformation
    vimt_transform_2d inverse() const;
    //: Returns change in transformed point when original point moved by dp.
    // Point dp: Movement from point
    // Returns: T(p+dp)-T(p)
    vgl_vector_2d<double>  delta(const vgl_point_2d<double> & p, const vgl_vector_2d<double> & dp) const;

    friend vimt_transform_2d operator*(const vimt_transform_2d&,
                                       const vimt_transform_2d&);

    short version_no() const;
    void print_summary(std::ostream&) const;
    void b_write(vsl_b_ostream& bfs) const;
    void b_read(vsl_b_istream& bfs);

    //: True if t is the same as this
    bool operator==(const vimt_transform_2d& t) const;

    //: Reduce to the simplest form possible.
    vimt_transform_2d& simplify(double tol =1e-10);

 private:

    double xx_,xy_,xt_,yx_,yy_,yt_,tx_,ty_,tt_;
    Form form_;

    // Notice the mutable here - take care if using threads!
    mutable double xx2_,xy2_,xt2_,yx2_,yy2_,yt2_,tx2_,ty2_,tt2_; // Inverse
    mutable bool inv_uptodate_;

    void calcInverse() const;
    void setCheck(int n1,int n2,const char* str) const;
};


std::ostream& operator<<(std::ostream&,const vimt_transform_2d& t);

//: Binary file stream output operator for pointer to class
void vsl_b_write(vsl_b_ostream& bfs, const vimt_transform_2d& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, vimt_transform_2d& b);

//: Stream output operator for class reference
void vsl_print_summary(std::ostream& os,const vimt_transform_2d& t);


#endif // vimt_transform_2d_h_
