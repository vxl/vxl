#ifndef betr_geo_box_3d_h_
#define betr_geo_box_3d_h_
//:
// \file
// \brief betr_geo_box_3d provides a geographic coordinate system for geometric objects
// \author J.L. Mundy
// \date April 29, 2016
//
//  the coordinates are based on the standard WGS84 spheroidal reference surface
//  the box is defined by a quadrilateral on the spheroid with max and min elevations
//  with respect to the spherioid. The only delicate issue is if the box spans the 0 or 180
//  longitude meridians or the box encloses one of the poles. The units are degrees and meters
//
#include <string>
#include <vgl/vgl_point_3d.h>
#include <vsph/vsph_sph_box_2d.h>
class betr_geo_box_3d
{
 public:
  betr_geo_box_3d(){
    sph_box_ = vsph_sph_box_2d(false);//assert units are degrees
    min_elv_=1.0; max_elv_ = 0.0;
  }

  bool is_empty() const{
    bool box_empty = sph_box_.is_empty();
    return box_empty || (min_elv_> max_elv_);
  }
  //: Add a geographic point to this box.
  // need to add a minimum of three points to define the interval
  // on the sphere
  void add(double lon, double lat, double el);

  // p.x() = lon, p.y() = lat, p.z()= elevation
  void add(vgl_point_3d<double> const& geo_pt);

  // add another geo box
  void add(betr_geo_box_3d const& geo_box);

  //: Does a geographic point lie inside or on the box
  bool contains(vgl_point_3d<double> const& geo_pt) const;
  bool contains(double lon, double lat, double el) const;
  //: access bounds
  double min_elv() const {return min_elv_;}
  double max_elv() const {return max_elv_;}
  double min_lon() const {return sph_box_.min_phi(false);}
  double max_lon() const {return sph_box_.max_phi(false);}
  double min_lat() const {return sph_box_.min_theta(false);}
  double max_lat() const {return sph_box_.max_theta(false);}
  vgl_point_3d<double> min_point() const{
    vgl_point_3d<double> p(min_lon(), min_lat(), min_elv());
    return p;
  }
  vgl_point_3d<double> max_point() const{
    vgl_point_3d<double> p(max_lon(), max_lat(), max_elv());
    return p;
  }

  //: auxillary point inside longitude interval to resolve arc ambiguity
  vgl_point_3d<double> aux_point() const;
 private:
  double min_elv_;
  double max_elv_;
  vsph_sph_box_2d sph_box_;
};
#endif //guard
