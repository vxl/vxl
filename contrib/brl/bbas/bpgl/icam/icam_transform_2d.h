// This is bbas/bpgl/icam/icam_transform_2d.h
#ifndef icam_transform_2d_h_
#define icam_transform_2d_h_
//:
// \file
// \brief An extension of vgl_h_matrix_2d to provide transform specializations
//        Partially copied from ihog_transform_2d
// \author Peter Vanroose
// \date October 10, 2010 (10/10/10)

#include <vbl/vbl_ref_count.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_2x3.h>

//: A quadrilateral region of an image
class icam_transform_2d : public vgl_h_matrix_2d<double>, public vbl_ref_count
{
 public:
  //: Defines form of transformation
  enum Form { Identity,
              Translation,
              RigidBody,
              Affine,
              Projective };
 private:
  Form form_;

 public:
  //: Default Constructor
  icam_transform_2d() : vgl_h_matrix_2d<double>(), vbl_ref_count(), form_(Identity) { vgl_h_matrix_2d<double>::set_identity(); }

  //: Constructor
  icam_transform_2d(vgl_h_matrix_2d<double> const& xform, Form form)
    : vgl_h_matrix_2d<double>(xform), form_(form) {}

  //: Destructor
  ~icam_transform_2d() override = default;

  //: Assignment operator
  icam_transform_2d& operator=(icam_transform_2d const& R) {
    vgl_h_matrix_2d<double>::set(R.get_matrix());
    form_ = R.form();
    return *this;
  }

  Form form() const {return form_;}
  void set_form(Form form) { form_ = form; }
  vnl_matrix_fixed<double,3,3> matrix() const { return vgl_h_matrix_2d<double>::get_matrix(); }

  void set(vnl_matrix_fixed<double,3,3> const& t_matrix) { vgl_h_matrix_2d<double>::set(t_matrix); }

  //: Sets transform using v (converse of params(v))
  void set(vnl_vector<double> const& v, Form); // Sets transform using v

  //: Returns the coordinates of the origin.
  // I.e. operator()(vgl_point_2d<double> (0,0))
  vgl_point_2d<double>  origin() const;

  //: Modifies the transformation so that operator()(vgl_point_2d<double> (0,0)) == p.
  // The rest of the transformation is unaffected.
  // If the transformation was previously the identity,
  // it becomes a translation.
  void set_origin(vgl_point_2d<double> const& new_origin);

  //: Sets to be 2D projective transformation using 3x3 matrix
  void set_projective(vnl_double_3x3 const&);

  //: Sets the transformation to be separable affine.
  // x' = s_x.x + t_x,  y' = s_y.y + t_y
  // \a s_x: Scaling in x
  // \a s_y: Scaling in y
  // \a t_x: Translation in x
  // \a t_y: Translation in y
  void set_zoom_only(double s_x, double s_y, double t_x, double t_y);

  //: Sets the transformation to be a zoom.
  // x' = s.x + t_x,  y' = s.y + t_y
  // \a   s: Scaling
  // \a t_x: Translation in x
  // \a t_y: Translation in y
  void set_zoom_only(double s, double t_x, double t_y) { set_zoom_only(s,s,t_x,t_y); }

  //: Sets the transformation to rotation then translation.
  // \a theta: rotation
  // \a t_x: Translation in x
  // \a t_y: Translation in y
  void set_rigid_body(double theta, double t_x, double t_y);

  //: Sets to be 2D affine transformation using 2x3 matrix
  void set_affine(vnl_double_2x3 const&);

  //: Sets to be 2D affine transformation T(x,y)=p+x.u+y.v
  void set_affine(vgl_point_2d<double> const& p,
                  vgl_vector_2d<double> const& u,
                  vgl_vector_2d<double> const& v);

  //: Calculates and returns inverse of this transformation
  icam_transform_2d inverse() const;

  //: Returns change in transformed point when original point moved by dp.
  // Point dp: Movement from point
  // Returns: T(p+dp)-T(p)
  vgl_vector_2d<double> delta(vgl_point_2d<double> const& p, vgl_vector_2d<double> const& dp) const;

  //: Fills v with parameters
  void params(vnl_vector<double>& v) const { params_of(v,form_); }

  //: Fills v with parameters of transform of type Form
  void params_of(vnl_vector<double>& v, Form) const;

  icam_transform_2d operator*(icam_transform_2d const& R) const
  {
    icam_transform_2d T(get_matrix()*R.get_matrix(), Identity);
    if      (form()==Projective ||R.form()==Projective)  T.set_form(Projective);
    else if (form()==Affine     ||R.form()==Affine)      T.set_form(Affine);
    else if (form()==RigidBody  ||R.form()==RigidBody)   T.set_form(RigidBody);
    else if (form()==Translation||R.form()==Translation) T.set_form(Translation);
    return T;
  }

  //: Applies transformation to (x,y)
  vgl_point_2d<double>  operator()(double x, double y) const;

  //: Returns transformation applied to point p
  vgl_point_2d<double>  operator()(vgl_point_2d<double> const& p) const { return operator()(p.x(),p.y()); }
};

#endif // icam_transform_2d_h_
