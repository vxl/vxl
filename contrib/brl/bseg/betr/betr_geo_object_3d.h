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
#include <vbl/vbl_ref_count.h>
#include <vpgl/vpgl_lvcs.h>
#include <vsol/vsol_spatial_object_3d.h>
#include <vsol/vsol_polygon_3d.h>
#include <vsol/vsol_polygon_3d_sptr.h>
#include <vsol/vsol_box_3d.h>
#include "betr_geo_box_3d.h"
#include "betr_geo_object_3d_sptr.h"
class betr_geo_object_3d : public vbl_ref_count
{
 public:
  betr_geo_object_3d()= default;
  betr_geo_object_3d(const vpgl_lvcs& lvcs):lvcs_(lvcs){}
  betr_geo_object_3d(vsol_spatial_object_3d_sptr so, vpgl_lvcs const& lvcs)
   : lvcs_(lvcs)
   , so_(so)
  {}

  betr_geo_object_3d(betr_geo_object_3d const& go){
    *this = go;}//maybe change later

  ~betr_geo_object_3d() override= default;
  // Data Access
  int id() const {return id_;}
  vsol_spatial_object_3d_sptr obj()const {return so_;}
  vpgl_lvcs lvcs() const {return lvcs_;}
  void set_id(int id){id_ = id;}
  void set_lvcs(vpgl_lvcs lvcs){lvcs_ = lvcs;}
  void set_spatial_obj(vsol_spatial_object_3d_sptr so){so_= so;}

  // Utilities
  betr_geo_box_3d bounding_box() const{
    betr_geo_box_3d box;
    if(!so_)
      return box;//empty box
    this->update_box_from_from_vsol_box(so_->get_bounding_box(), box);
    return box;
  }
  //: The polygon at the base of a 3-d mesh object. If the object is a single polygon, it is returned.
  // the vertex coordinates are global lat, lon, elev
  vsol_polygon_3d_sptr base_polygon();
  friend std::ostream& operator << (std::ostream& os, const betr_geo_object_3d& geo_obj);
protected:
  void update_box_from_from_vsol_box(const vsol_box_3d_sptr& sbox, betr_geo_box_3d& box) const;
  //Members
  int id_;
  vpgl_lvcs lvcs_;     // Local Vertical Coordinate System
  vsol_spatial_object_3d_sptr so_;
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS betr_geo_object_3d.
