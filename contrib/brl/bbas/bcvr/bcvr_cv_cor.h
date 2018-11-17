// This is brl/bbas/bcvr/bcvr_cv_cor.h
//---------------------------------------------------------------------
#ifndef bcvr_cv_cor_h_
#define bcvr_cv_cor_h_
//:
// \file
// \brief a class to hold the curve correspondence as vectors of points
//        WARNING: This class has to be revised to work with the correspondence of "open curves"
//
// \author
//  O.C. Ozcanli - Nov 23, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vector>
#include <utility>
#include <iostream>
#include <string>
#include <vbl/vbl_ref_count.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>
#include <vgl/vgl_point_2d.h>
#include <bsol/bsol_intrinsic_curve_2d_sptr.h>

#include <vsol/vsol_polygon_2d_sptr.h>

// either use the constructor or set the point vectors directly
class bcvr_cv_cor : public vbl_ref_count
{
 public:
  //: constructors
  bcvr_cv_cor();
  bcvr_cv_cor(const bsol_intrinsic_curve_2d_sptr& c1,
              const bsol_intrinsic_curve_2d_sptr& c2,
              std::vector<std::pair<int,int> >& map,
              int n1);

  ~bcvr_cv_cor() override  { poly1_ = nullptr; poly2_ = nullptr; };

  //access functions
  std::vector<vgl_point_2d<double> >& get_contour_pts1() { return pts1_; }
  std::vector<vgl_point_2d<double> >& get_contour_pts2() { return pts2_; }

  void set_contour_pts1(const std::vector<vgl_point_2d<double> >& pts1) { pts1_ = pts1; }
  void set_contour_pts2(const std::vector<vgl_point_2d<double> >& pts2) { pts2_ = pts2; }

  //: given s1 on curve1 return alpha(s1) on curve2.
  // interpolate alpha (correspondence function) piecewise linearly using the final map
  // alpha has vertical jumps, in those cases assign the jumped value to the point of jump on curve1
  // i.e. in the figure below, return s2 as alpha(s1) (not s)
  // \verbatim
  //   s2|  ----
  //     |  |
  //   s | -
  //     |/
  //     --------
  //        s1
  // \endverbatim
  //
  // do the same thing for reverse operation
  double get_arclength_on_curve2(double s1);
  //: given s2 on curve2 return alpha_inverse(s2) on curve1
  double get_arclength_on_curve1(double s2);

  double get_length1() { return length1_; }
  double get_length2() { return length2_; }

  //: write points to a file
  bool write_correspondence(const std::string& file_name, int increment = 1);

  vsol_polygon_2d_sptr get_poly1() { return poly1_; }
  vsol_polygon_2d_sptr get_poly2() { return poly2_; }

  void set_final_cost(double cost) { final_cost_ = cost; }
  void set_final_norm_cost(double cost) { final_norm_cost_ = cost; }

  void set_open_curve_matching(bool val) { open_curve_matching_ = val; }  // needs to true explicitly for open curve matching

  //-----------------------
  //  BINARY I/O METHODS  |
  //-----------------------

  //: Serial I/O format version
  virtual unsigned version() const {return 1;}

  //: Return a platform independent string identifying the class
  virtual std::string is_a() const {return "bcvr_cv_cor";}

  //: determine if this is the given class
  virtual bool is_class(std::string const& cls) const { return cls==is_a(); }

  //: Binary save self to stream.
  virtual void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  virtual void b_read(vsl_b_istream &is);

  //: data
  //  polygons used to get sillhuoette correspondence
  //  (might be line fitted versions of input polygons
  //  which are inputted to dbcvr_clsd_cvmatch)
  vsol_polygon_2d_sptr poly1_;
  vsol_polygon_2d_sptr poly2_;

  // output anchor points of correspondece
  // correspondence is piewise linear between these anchor points
  std::vector<vgl_point_2d<double> > pts1_;
  std::vector<vgl_point_2d<double> > pts2_;

  std::vector<double> arclengths1_, arclengths2_;

  double length1_, length2_;
  double final_cost_, final_norm_cost_;

  bool open_curve_matching_;
};

#endif // bcvr_cv_cor_h_
