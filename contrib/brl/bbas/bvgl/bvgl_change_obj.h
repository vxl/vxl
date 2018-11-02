#ifndef bvgl_change_obj_h_
#define bvgl_change_obj_h_
//:
// \file
// \brief Helper classes to annotate polygonal regions in images
// \author Ozge C. Ozcanli ozge@lems.brown.edu
// \date   September 26, 2008
//
// \verbatim
//  Modifications
//   <None yet>
// \endverbatim

#include <iostream>
#include <string>
#include <utility>
#include <vgl/vgl_polygon.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_ref_count.h>
#include <vsl/vsl_binary_io.h>

class bvgl_change_obj: public vbl_ref_count
{
 public:
  bvgl_change_obj(vgl_polygon<double> const& poly) : class_("unknown"), poly_(poly) {}
  bvgl_change_obj(vgl_polygon<double> const& poly, std::string cls) : class_(std::move(cls)), poly_(poly) {}
  bvgl_change_obj(const bvgl_change_obj& other);
  bvgl_change_obj() = default;
  ~bvgl_change_obj() override = default;

  //: binary IO write
  void b_write(vsl_b_ostream& os);

  //: binary IO read
  void b_read(vsl_b_istream& is);

  //: Return IO version number;
  unsigned char version() const;

  vgl_polygon<double> poly() const {return poly_; }

  //: return the centroid of the polygon
  void centroid(double &x, double& y) const;

  std::string type() const { return class_; }
  //void xml_read();
  //void xml_write();
 private:
  std::string class_;
  vgl_polygon<double> poly_;
};

#endif // bvgl_change_obj_h_
