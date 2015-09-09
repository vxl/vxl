#ifndef boxm2_vecf_spline_field_h_
#define boxm2_vecf_spline_field_h_
//:
// \file
// \brief  Interpolation of a 3-d vector field along a 3-d spline curve
//
// \author J.L. Mundy
// \date   3 Sept. 2015
//
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_cubic_spline_3d.h>
class boxm2_vecf_spline_field{
 public:
  boxm2_vecf_spline_field(){}
  // number of spline knots must equal the number of field knots
 boxm2_vecf_spline_field(vgl_cubic_spline_3d<double> const& spline, vcl_vector<vgl_vector_3d<double> > const& field_knots):
  generating_spline_(spline), field_knots_(field_knots){if(static_cast<unsigned>(field_knots_.size()) != spline.n_knots())
      vcl_cout << "FATAL! - field knots must be the same size as spline knots\n";
  }
  //: accessors
  double max_t() const{ return generating_spline_.max_t();}
  double n_knots() const { return generating_spline_.n_knots();}

  //: field value at t, where  0 <= t < n-1 and n is the number of knots
  vgl_vector_3d<double> operator ()(double t) const;

  //: apply field to generating spline
  vgl_cubic_spline_3d<double> apply_field() const;

 private:
  vgl_cubic_spline_3d<double> generating_spline_;
  vcl_vector<vgl_vector_3d<double> > field_knots_;
};
#endif// boxm2_vecf_spline_field
