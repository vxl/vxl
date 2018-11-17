//This is brl/bbas/volm/conf/volm_conf_object.h
#ifndef volm_conf_object_h_
#define volm_conf_object_h_
//:
// \file A class to represent an object that is defined by its position (relative to some coordinates), land type and other attributes
//  Note the default location is defined by cylindrical coordinate system

// \author Yi Dong
// \date July 16, 2014
// \verbatim
//   Modifications
//    Yi Dong     SEP--2014    added height attribute
// \endverbatim

#include <iostream>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>

class volm_conf_object;
typedef vbl_smart_ptr<volm_conf_object> volm_conf_object_sptr;


class volm_conf_object : public vbl_ref_count
{
public:
  //: default constructor
  volm_conf_object() :
    theta_(0.0f),
    dist_(0.0f),
    height_(0.0f),
    land_(0)
  {}

  //: constructor from location theta, dist, land_id
  volm_conf_object(float const& theta,
                   float const& dist,
                   float const& height,
                   unsigned char const& land);

  volm_conf_object(double const& theta,
                   double const& dist,
                   double const& height,
                   unsigned char const& land);

  //: constructor from a 2-d points, a height value and land id
  volm_conf_object(vgl_point_2d<float> const& pt,
                   float const& height,
                   unsigned char const& land);

  volm_conf_object(vgl_point_2d<double> const& pt,
                   double const& height,
                   unsigned char const& land);

  //: constructor from a 3-d points (note the dist are the ground distance, i.e. calculated by pt.x() and pt.y())
  volm_conf_object(vgl_point_3d<float> const& pt,
                   unsigned char const& land);

  volm_conf_object(vgl_point_3d<double> const& pt,
                   unsigned char const& land);

  //: destructor
  ~volm_conf_object() override = default;

  //: access
  float theta()  const        { return theta_; }
  float dist()   const        { return dist_;   }
  float height() const        { return height_; }
  unsigned char land()  const { return land_;   }
  float theta_in_deg() const;

  //: return the location as Cartesian
  float x() const;
  float y() const;
  vgl_point_2d<float> loc() const;

  //: check whether the two configuration object is same or not
  bool is_same(volm_conf_object const& other);
  bool is_same(const volm_conf_object_sptr& other_sptr);
  bool is_same(volm_conf_object const* other_ptr);

  //: print method
  void print(std::ostream& os) const
  {
    os << "volm_conf_object -- theta: " << theta_ << " (" << this->theta_in_deg() << " degree), dist: " << dist_
       << ", height: " << height_
       << ", land: " << (int)land_ << ")\n";
  }

  //: visualization method for configuration index and values are the indices relative to geo-location defined by lon and lat
  static bool write_to_kml(double const& lon, double const& lat, std::vector<volm_conf_object>& values, std::string const& kml_file);

  // ================  binary I/O ===================
  //: version
  unsigned char version() const { return (unsigned char)2; }
  //: binary IO write
  void b_write(vsl_b_ostream& os) const;
  //: binary IO read
  void b_read(vsl_b_istream& is);

private:
  float theta_;             // location coordinate theta
  float dist_;              // location coordinate radius
  float height_;
  unsigned char land_;      // object land type id
};

void vsl_b_write(vsl_b_ostream& os, volm_conf_object const&     obj);
void vsl_b_write(vsl_b_ostream& os, volm_conf_object const* obj_ptr);
void vsl_b_write(vsl_b_ostream& os, volm_conf_object_sptr const& obj_sptr);

void vsl_b_read(vsl_b_istream& is, volm_conf_object&           obj);
void vsl_b_read(vsl_b_istream& is, volm_conf_object*&      obj_ptr);
void vsl_b_read(vsl_b_istream& is, volm_conf_object_sptr& obj_sptr);

void vsl_print_summary(std::ostream& os, volm_conf_object const& obj);


#endif // volm_conf_object
