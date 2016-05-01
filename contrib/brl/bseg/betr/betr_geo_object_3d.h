#ifndef betr_geo_object_3d_h_
#define betr_geo_object_3d_h_
//:
// \file
// \brief a 3-d geographic object with associated bounding box
// \author J.L. Mundy
// \date April 29, 2016
//
#include <string>
#include <iostream>
#include <vpgl/vpgl_lvcs.h>
#include <vsol/vsol_spatial_object_3d.h>
#include <vsol/vsol_box_3d.h>
#include "betr_geo_box_3d.h"
class betr_geo_object_3d
{
 public:
  betr_geo_object_3d(){}
  betr_geo_object_3d(const vpgl_lvcs& lvcs):lvcs_(lvcs){}
  betr_geo_object_3d(vsol_spatial_object_3d_sptr so, const std::string& country, const std::string& region,
                     const std::string& location, vpgl_lvcs const& lvcs):so_(so), country_(country),
    region_(region), location_(location), lvcs_(lvcs){}
    
  betr_geo_object_3d(betr_geo_object_3d const& go){
    *this = go;}//maybe change later
  
  ~betr_geo_object_3d(){}
  // Data Access
  int id() const {return id_;}
  vsol_spatial_object_3d_sptr obj()const {return so_;}
  std::string country() const {return country_;}
  std::string region() const {return region_;}
  std::string location() const {return location_;}
  vpgl_lvcs lvcs() const {return lvcs_;}
  void set_id(int id){id_ = id;}
  void set_lvcs(vpgl_lvcs lvcs){lvcs_ = lvcs;}
  void set_country(const std::string& country) {country_ = country;}
  void set_region(const std::string& region) {region_ = region;}
  void set_location(const std::string& location) {location_ = location;}
  void set_spatial_obj(vsol_spatial_object_3d_sptr so){so_= so;}

  // Utilities
  betr_geo_box_3d bounding_box() const{
    betr_geo_box_3d box;
    if(!so_)
      return box;//empty box
    this->update_box_from_from_vsol_box(so_->get_bounding_box(), box);
    return box;
  }
                                       
  friend std::ostream& operator << (std::ostream& os, const betr_geo_object_3d& geo_obj);
protected:
  void update_box_from_from_vsol_box(vsol_box_3d_sptr sbox, betr_geo_box_3d& box) const;
  //Members
  int id_;
  std::string country_; //e.g. Continental United States
  std::string region_;  //e.g. CapitalDistrict
  std::string location_; //e.g. Schenectady AFB
  vpgl_lvcs lvcs_;     // Local Vertical Coordinate System
  vsol_spatial_object_3d_sptr so_;
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS betr_geo_object_3d.
