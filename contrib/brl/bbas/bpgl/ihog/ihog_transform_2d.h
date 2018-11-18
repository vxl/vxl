// This is bbas/bpgl/ihog/ihog_transform_2d.h
#ifndef ihog_transform_2d_h_
#define ihog_transform_2d_h_
//:
// \file
// \brief An extension of vgl_h_matrix_2d to provide transform specializations
// \author Matt Leotta
// \date 4/27/04
//
// \verbatim
//  Modifications
//  G.Tunali 8/2010 Removed dependency on vimt
// \endverbatim

#include <iostream>
#include <vbl/vbl_ref_count.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_2x3.h>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: A quadrilateral region of an image
class ihog_transform_2d : public vgl_h_matrix_2d<double>, public vbl_ref_count
{
 public:
  //: Defines form of transformation
  enum Form { Identity,
              Translation,
              RigidBody,
              Affine,
              Projective
  };

  //: Default Constructor
  ihog_transform_2d()
    : vgl_h_matrix_2d<double>() { set_identity(); form_=Identity;}

  //: Constructor
  ihog_transform_2d(const vgl_h_matrix_2d<double>& xform, Form form)
    : vgl_h_matrix_2d<double>(xform), form_(form) {}

  //: Destructor
  ~ihog_transform_2d() override = default;

  Form form() const {return form_;}
  void set_form(const Form form) {form_ = form;}
  //vnl_double_3x3 matrix() const { return t12_matrix_; } -- use get_matrix() instead!

  void set(vnl_double_3x3 const& t_matrix) { vgl_h_matrix_2d<double>::set(t_matrix); }

  //: Sets transform using v (converse of params(v))
  void set(const vnl_vector<double>& v, Form); // Sets transform using v

  //: Returns the coordinates of the origin.
  // I.e. operator()(vgl_point_2d<double> (0,0))
  vgl_point_2d<double>  origin() const;

  //: Modifies the transformation so that operator()(vgl_point_2d<double> (0,0)) == p.
  // The rest of the transformation is unaffected.
  // If the transformation was previously the identity,
  // it becomes a translation.
  void set_origin( const vgl_point_2d<double> & );

  //: Sets to be 2D projective transformation
  void set_projective(vnl_double_3x3 const&);

  //: Sets the transformation to be separable affine.
  // x' = s_x.x + t_x,  y' = s_y.y + t_y
  // s_x: Scaling in x
  // s_y: Scaling in y
  // t_x: Translation in x
  // t_y: Translation in y
  void set_zoom_only(double s_x, double s_y, double t_x, double t_y);

  //: Sets the transformation to be a zoom.
  // x' = s.x + t_x,  y' = s.y + t_y
  //   s: Scaling
  // t_x: Translation in x
  // t_y: Translation in y
  void set_zoom_only(double s, double t_x, double t_y) { set_zoom_only(s,s,t_x,t_y); form_ = Translation; }

  //: Sets the transformation to rotation then translation.
  // theta: rotation
  // t_x: Translation in x
  // t_y: Translation in y
  void set_rigid_body(double theta, double t_x, double t_y);

  //: Sets to be 2D affine transformation using 2x3 matrix
  void set_affine(vnl_double_2x3 const&);

  //: Sets to be 2D affine transformation T(x,y)=p+x.u+y.v
  void set_affine(const vgl_point_2d<double> & p,
                  const vgl_vector_2d<double> & u,
                  const vgl_vector_2d<double> & v);

  void set_translation_only(double t_x, double t_y) { set_zoom_only(1,t_x,t_y); form_ = Translation; }

  //: Calculates inverse of this transformation
  ihog_transform_2d inverse() const;

  //: Returns change in transformed point when original point moved by dp.
  // Point dp: Movement from point
  // Returns: T(p+dp)-T(p)
  vgl_vector_2d<double>  delta(const vgl_point_2d<double> & p, const vgl_vector_2d<double> & dp) const;

  //: Fills v with parameters
  void params(vnl_vector<double>& v) const { params_of(v,form_); }
  //: Fills v with parameters of transform of type Form
  void params_of(vnl_vector<double>& v, Form) const;

  //: Applies transformation to (x,y)
  vgl_point_2d<double>  operator()(double x, double y) const;
  //: Returns transformation applied to point p
  vgl_point_2d<double>  operator()(const vgl_point_2d<double> & p) const { return operator()(p.x(),p.y()); }

  ihog_transform_2d& operator=(const ihog_transform_2d&);

  void b_write(vsl_b_ostream& bfs) const;
  void b_read(vsl_b_istream& bfs);
  short version_no() const { return 1;}
 private:
  Form form_;
};


inline ihog_transform_2d operator*(const ihog_transform_2d& L, const ihog_transform_2d& R)
{
  ihog_transform_2d T;
  vnl_double_3x3 m(L.get_matrix()*R.get_matrix());
  T.set(m);
  if (L.form()==ihog_transform_2d::Projective||
      R.form()==ihog_transform_2d::Projective)
  { T.set_form(ihog_transform_2d::Projective); return T; }
  if (L.form()==ihog_transform_2d::Affine||
      R.form()==ihog_transform_2d::Affine)
  { T.set_form(ihog_transform_2d::Affine); return T; }
  if (L.form()==ihog_transform_2d::RigidBody||
      R.form()==ihog_transform_2d::RigidBody)
  { T.set_form(ihog_transform_2d::RigidBody); return T; }
  if (L.form()==ihog_transform_2d::Translation||
      R.form()==ihog_transform_2d::Translation)
  { T.set_form(ihog_transform_2d::Translation); return T; }
  return T;
}

//: Binary save ihog_transform_2d* to stream.
inline void vsl_b_write(vsl_b_ostream &os, ihog_transform_2d const* t)
{
  if (t==nullptr) {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else {
    vsl_b_write(os,true); // Indicate non-null pointer stored
    t->b_write(os);
  }
}


//: Binary load ihog_transform_2d* from stream.
inline void vsl_b_read(vsl_b_istream &is, ihog_transform_2d* &t)
{
  delete t;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr) {
    t = new ihog_transform_2d();
    t->b_read(is);
  }
  else
    t = nullptr;
}


//: Print an ASCII summary to the stream
inline void vsl_print_summary(std::ostream &os, const ihog_transform_2d* t)
{
  os << *t;
}

#endif // ihog_transform_2d_h_
